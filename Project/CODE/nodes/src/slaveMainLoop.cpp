#include "utils/FuncThread.hpp"
//
extern "C" {
#include "SEEKFREE_IPS114_SPI.h"
}

#include <algorithm>
#include <cmath>

#include "Camera.hpp"
#include "apriltag/RectSender.hpp"
#include "apriltag/apriltag.hpp"
#include "apriltag/reconcileRects.hpp"
#include "apriltag/undisort.hpp"
#include "apriltag/visualization.hpp"
#include "devices.hpp"
#include "edge_detect/A4Detect.hpp"
#include "edge_detect/show_edge.hpp"
//
#include "RectConfig.hpp"

using namespace imgProc;
using namespace imgProc::apriltag;
using namespace imgProc::edge_detect;

static int32_t pre_time;
static uint8_t* img;

static inline void A4Detect() {
    static SerialIO::TxArr<float, target_coords_maxn * 2, true> a4_tx(32, "a4_tx");

    bool res = A4Detect(img, 7, 5, 50, 100);
    if (res) {
        a4_tx.txFinished(-1);
        a4_tx.setArr(target_coords_corr[0], target_coords_cnt * 2);
        uart3.send(a4_tx);
    }
    show_edge(img);  // ��ʾ��ԵͼƬ
}

static inline void FindRect() {
    static RectSender rectSender(33);

    // bool show_thresh = slave_key[0].get();  // �Ƿ���ʾ��ֵ��ͼ
    bool show_thresh = 0;
    bool visualize = !show_thresh && slave_switch[1].get();  // �Ƿ���ʾЧ��

    rects_t& rects = find_rects(img, min_magnitude,
                                show_thresh ? apriltag_detect_visualize_flag::threshim : apriltag_detect_visualize_flag::None);
    if (visualize) plot_rects(img, rects, GREEN);

    reconcileRects(rects);

    rectSender.send_to(rects, uart3);

    if (visualize) {
        plot_rects(img, rects, RED);
        show_plot_grayscale(img);
    }
}

static void slaveMainLoopEntry() {
    int32_t pre_time = rt_tick_get();

    for (;;) {
        img = camera.snapshot();

        if (slave_switch[2].get()) {
            show_grayscale(img);
        } else {
            A4Detect();
            // FindRect();
        }

        camera.release();
        int32_t cur_time = rt_tick_get();
        ips114_showint32(188, 0, cur_time - pre_time, 3);  // ��ʾ��ʱ/ms
        pre_time = cur_time;
    }
}

bool slaveMainLoopNode() { return FuncThread(slaveMainLoopEntry, "slaveMainLoop", 4096, 2, 1000); }
