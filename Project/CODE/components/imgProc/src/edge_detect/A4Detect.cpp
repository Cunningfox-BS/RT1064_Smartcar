#include "edge_detect/A4Detect.hpp"

#include <bitset>
#include <utility>

#include "apriltag/fmath.hpp"
#include "apriltag/internal/StaticBuffer.hpp"
#include "apriltag/internal/fit_quad.hpp"
#include "apriltag/internal/homography.hpp"
#include "apriltag/visualization.hpp"
#include "bresenham.hpp"
#include "devices.hpp"
#include "edge_detect/canny.hpp"
#include "edge_detect/show_edge.hpp"
#include "imgProc/CoordStack.hpp"
extern "C" {
#include "SEEKFREE_IPS114_SPI.h"
}

#define debug_ips(s) ips114_showstr(188, 3, s)

namespace imgProc {
using namespace apriltag;
namespace edge_detect {
using std::pair;

apriltag::quad target_quad;
Coordinate target_coords[target_coords_maxn];
int target_coords_cnt;
float_t target_coords_corr[target_coords_maxn][2];

#define PIXEL(img, x, y) (*(img + (y)*M + (x)))
#define DRAW(img, x, y) (*(img + ((y) & ~3) * M + ((x) & ~3)))
#define CIRCLE(img, x, y, r, color)                                                            \
    drawCircle(int(y) >> 2, int(x) >> 2, (r), [img](int i, int j) {                            \
        if (0 <= i && i < N / 4 && 0 <= j && j < M / 4) *(img + ((i * M + j) << 2)) = (color); \
    })

constexpr int dxy8[][2]{{1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}};
constexpr int dxy4[][2]{{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
constexpr int dxy9[][2]{{0, 0}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}};

// ����
static inline void dilation(uint8_t* img) {
    static std::bitset<N * M> binim;
    binim.reset();
    // ����Ҫ�����ڱ�Ե�ĵ�
    for (int y = 1; y < N - 1; ++y)
        for (int x = 1; x < M - 1; ++x)
            for (auto [dx, dy] : dxy9)
                if (PIXEL(img, x + dx, y + dy) == 255) {
                    binim.set(y * M + x);
                    break;
                }
    for (int i = 0; i < N * M; ++i) *img++ = binim.test(i) ? 255 : 0;
}

// �ҵ��߿��Ͼ���������Զ�ĵ�
static inline pair<int, int> find_farthest(uint8_t* img, bool visualize = false) {
    constexpr float start_dist = 50;

    int res_i = -1, res_j = -1;
    int res_dist2 = 0;
    for (int degree = 0; degree < 360; degree += 10) {
        float x = M / 2, y = N / 2;
        float rad = degree * (3.14159265f / 180);
        float dx = cosf(rad), dy = sinf(rad);
        x += dx * start_dist, y += dy * start_dist;
        int i, j, dist2;
        for (;;) {
            x += dx, y += dy;
            i = y, j = x;
            if (!(0 <= i && i < N && 0 <= j && j < M)) break;
            if (*(img + (i * M + j)) == 255) {
                dist2 = (i - N / 2) * (i - N / 2) + (j - M / 2) * (j - M / 2);
                if (dist2 > res_dist2) {
                    res_dist2 = dist2;
                    res_i = i, res_j = j;
                }
                break;
            }
            if (visualize) *(img + ((i & ~3) * M + (j & ~3))) = 2;  // RED
        }
    }
    if (visualize) CIRCLE(img, res_j, res_i, 6, 3);
    return {res_i, res_j};
}

// 8����׷�ٱ߿�
static inline pair<Coordinate*, int> edgeTrace(uint8_t* img, int X, int Y, bool visualize = false) {
    int half_size = staticBuffer.free() / 2;
    int n = 0;
    Coordinate* coords = (Coordinate*)staticBuffer.allocate(half_size);
    CoordStack stack;
    stack.push(X, Y);
    PIXEL(img, X, Y) = 0;
    while (!stack.empty()) {
        auto [x, y] = stack.pop();
        coords[n].x = x, coords[n].y = y, ++n;
        for (int i = 0, u, v; i < 8; ++i) {
            u = x + dxy8[i][0], v = y + dxy8[i][1];
            if (u == 0 || u == M - 1 || v == 0 || v == N - 1) {
                while (!stack.empty()) stack.pop();
                staticBuffer.pop(half_size);
                return {nullptr, 0};
            }
            if (PIXEL(img, u, v) == 255) {
                PIXEL(img, u, v) = 0;
                stack.push(u, v);
            }
        }
    }
    staticBuffer.pop(half_size);
    staticBuffer.allocate(n * sizeof(Coordinate));
    if (visualize)
        for (int i = 0; i < n; ++i) DRAW(img, coords[i].x, coords[i].y) = 2;
    return {coords, n};
}

// ���ڵ�
static inline bool dfs_black(uint8_t* img, int x, int y) {
    if (target_coords_cnt >= target_coords_maxn) return false;
    CoordStack stack;
    stack.push(x, y);
    int cnt = 0, xmax = x, xmin = x, ymax = y, ymin = y;
    while (!stack.empty()) {
        auto [x, y] = stack.pop();
        if (0 == x || x == M - 1 || 0 == y || y == N - 1) {
            while (!stack.empty()) stack.pop();
            return false;
        }
        ++cnt;
        if (x > xmax) xmax = x;
        else if (x < xmin)
            xmin = x;
        if (y > ymax) ymax = y;
        else if (y < ymin)
            ymin = y;
        for (int t = 0, u, v; t < 4; ++t) {
            u = x + dxy4[t][0], v = y + dxy4[t][1];
            if (PIXEL(img, u, v) == 255) {  // ֻ�е���һ�����Ǻڵ�ʱ����ջ
                PIXEL(img, u, v) = 0;       // �Ѻڵ�ĳɰ׵�
                stack.push(u, v);
            }
        }
    }
    if (cnt < 8 || xmax - xmin > 100 || ymax - ymin > 100) return false;
    target_coords[target_coords_cnt].x = (xmax + xmin) >> 1;
    target_coords[target_coords_cnt].y = (ymax + ymin) >> 1;
    ++target_coords_cnt;
    return true;
}

// ̽���׵�
static inline bool dfs_white(uint8_t* img) {
    target_coords_cnt = 0;
    int x = M / 2, y = N / 2;
    CoordStack stack;
    stack.push(x, y);
    while (!stack.empty()) {
        auto [x, y] = stack.pop();  //
        if (0 == x || x == M - 1 || 0 == y || y == N - 1) {
            while (!stack.empty()) stack.pop();
            return false;
        }
        for (int t = 0, u, v; t < 4; ++t) {
            u = x + dxy4[t][0], v = y + dxy4[t][1];
            if (PIXEL(img, u, v) == 255) {  // �����һ�����Ǻڵ�����dfs_black�������Ŀ���
                PIXEL(img, u, v) = 0;
                if (!dfs_black(img, u, v)) {
                    while (!stack.empty()) stack.pop();
                    return false;
                }
            }
            if (PIXEL(img, u, v) <= 1) {  // Cannyʱ 0: �Ǳ߽� 1: ���ڵ���ֵ�����Ǳ߽�ĵ�
                PIXEL(img, u, v) = 10;    // ������ֵ����ʾ�ѷ���
                stack.push(u, v);
            }
        }
    }
    return true;
}

bool A4Detect(uint8_t* img, apriltag::float_t borderWidth, apriltag::float_t borderHeight, int low_thresh, int high_thresh) {
    staticBuffer.reset();

    debug_ips(":0");

    gvec_t* g = canny(img, low_thresh, high_thresh);  // ��Ե���
#define free_g staticBuffer.pop(N* M * sizeof(*g))

    // dilation(img);

    debug_ips(":1");

    auto [y, x] = find_farthest(img);  // �ҵ���Զ�ĵ�
    if (y == -1) {
        free_g;
        return false;
    }

    debug_ips(":2");

    auto [coords, sz] = edgeTrace(img, x, y);
#define free_coords staticBuffer.pop(sz * sizeof(*coords))

    debug_ips(":3");

    ips114_showint32(188, 1, sz, 4);
    if (sz < 1000) {
        free_coords;
        free_g;
        return false;
    }

    bool fit_res = fit_quad_simple(coords, sz, target_quad, g);

    debug_ips(":4");

    if (fit_res) {
        // ���߿��������ص�3x3��Χ�ڱ�ǳɱ߽�
        for (int i = 0; i < sz; ++i)
            for (int x = coords[i].x - 1; x <= coords[i].x + 1; ++x)
                for (int y = coords[i].y - 1; y <= coords[i].y + 1; ++y) PIXEL(img, x, y) = 3;
    }
    free_coords;
    free_g;
    if (!fit_res) return false;

    // �����Ŀ�ʼdfs���Һڵ�
    if (!dfs_white(img)) return false;
    ips114_showint32(188, 2, target_coords_cnt, 3);

    debug_ips(":5");

    // �����
    for (int i = 0; i < target_coords_cnt; ++i) CIRCLE(img, target_coords[i].x, target_coords[i].y, 3, 2);
    for (int i = 0; i < 4; ++i) CIRCLE(img, target_quad.p[i][0], target_quad.p[i][1], 6, 3);

    // ����͸�ӱ任����
    // clang-format off
    apriltag::float_t corr_arr[4][4]{   // fit_quad_simple���õ���4���ǵ�˳���ǹ̶���
        {target_quad.p[0][0], target_quad.p[0][1], borderWidth, borderHeight},  // ���Ͻ�
        {target_quad.p[1][0], target_quad.p[1][1], borderWidth, 0,          },  // ���½�
        {target_quad.p[2][0], target_quad.p[2][1], 0,           0,          },  // ���½�
        {target_quad.p[3][0], target_quad.p[3][1], 0,           borderHeight},  // ���Ͻ�
    };
    // clang-format on
    homography_compute2(target_quad.H, corr_arr);

    debug_ips(":6");

    // ������ת��Ϊ��������
    for (int i = 0; i < target_coords_cnt; ++i) {
        homography_project(target_quad.H, target_coords[i].x, target_coords[i].y, target_coords_corr[i],
                           target_coords_corr[i] + 1);
    }

    debug_ips(":7");

    return true;
}

void draw_corr(apriltag::float_t target_coords_corr[][2], int target_coords_cnt, apriltag::float_t borderWidth,
               apriltag::float_t borderHeight, uint16_t color) {
    float k = std::min((M / 4) / borderWidth, (N / 4) / borderHeight);
    for (int I = 0; I < target_coords_cnt; ++I) {
        int i = (borderHeight - target_coords_corr[I][1]) * k, j = target_coords_corr[I][0] * k;
        drawCircle(i, j, 3, [color](int i, int j) {
            if (0 <= i && i < N / 4 && 0 <= j && j < M / 4) ips114_drawpoint(j, i, color);
        });
    }
}

}  // namespace edge_detect
}  // namespace imgProc
