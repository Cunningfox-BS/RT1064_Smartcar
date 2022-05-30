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
//
#include "RectConfig.hpp"

namespace imgProc {
namespace apriltag {

static void testFindRectEntry() {
    // AT_DTCM_SECTION_ALIGN(static uint8_t img[N * M], 64);
    static RectSender rectSender(33);

    int32_t pre_time = rt_tick_get();

    for (;;) {
        bool visualize = slave_switch[2].get();  // ���뿪�ؾ����Ƿ���п��ӻ�

        // uint8_t* src = camera.snapshot();
        // undisort_I(src, img);  // ����ͼ�����
        uint8_t* img = camera.snapshot();

        rects_t& rects = find_rects(img, min_magnitude);
        if (visualize) plot_rects(img, rects, GREEN);

        reconcileRects(rects);

        rectSender.send_to(rects, uart3);

        if (visualize) plot_rects(img, rects, RED);

        if (visualize) show_plot_grayscale(img);

        camera.release();  // �ͷ�ͼƬ

        int32_t cur_time = rt_tick_get();
        ips114_showint32(188, 0, cur_time - pre_time, 3);  // ��ʾ��ʱ/ms
        pre_time = cur_time;
    }
}
}  // namespace apriltag
}  // namespace imgProc

bool testFindRectNode() { return FuncThread(imgProc::apriltag::testFindRectEntry, "testFindRect", 4096, 2, 1000); }
