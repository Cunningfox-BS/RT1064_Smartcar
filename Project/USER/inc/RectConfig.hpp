#ifndef _RectConfig_hpp
#define _RectConfig_hpp

#include "apriltag/config.hpp"

namespace imgProc {
namespace apriltag {

// ���͵����ڵ����Rect����
constexpr int max_rect_cnt = 5;

// ��ЧRect����С�߽��ݶȺ�
constexpr float_t min_magnitude = 7000;

// Rect�߳��ķ�Χ, ��λ��m
constexpr float_t min_rect_size = 0.09;
constexpr float_t max_rect_size = 0.15;

// Rect�������߼нǷ�Χ, ��λ�Ƕ�
constexpr float_t min_rect_angle = 70;
constexpr float_t max_rect_angle = 110;

// ��������̵�͸�ӱ任��Ӧ�����
// clang-format off
constexpr float_t Cam2Base_corr[4][4]{   // {{ͼ��x_i, ͼ��y_i, ����x_i, ����y_i}, 0<=i<=3}
    {40,  451, 0.149,  0.293},  // ���½�
    {699, 453, 0.163, -0.353},  // ���½�
    {713, 24,  0.843, -0.657},  // ���Ͻ�
    {33,  12,  0.802,  0.579},  // ���Ͻ�
};
// clang-format on

}  // namespace apriltag
}  // namespace imgProc

#endif  // _RectConfig_hpp