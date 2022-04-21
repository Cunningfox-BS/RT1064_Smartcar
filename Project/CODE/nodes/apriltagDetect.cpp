#include "utils/FuncThread.hpp"
//
extern "C" {
#include "SEEKFREE_IPS114_SPI.h"
#include "SEEKFREE_MT9V03X_CSI.h"
#include "common.h"
#include "fsl_debug_console.h"
#include "zf_gpio.h"
}

#include "apriltag/apriltag.hpp"
#include "apriltag/apriltag_pose.hpp"
#include "apriltag/tag25h9.hpp"
#include "apriltag/undisort.hpp"
#include "apriltag/visualization.hpp"
#include "apriltag/visualization_pose.hpp"
#include "devices.hpp"

// �����tag�Ĳ��������ʶ���õĲ�����"apriltag/config.hpp"��
#include "ApriltagConfig.hpp"

static void apriltagDetectEntry() {
    using namespace imgProc::apriltag;
    AT_DTCM_SECTION_ALIGN(static uint8_t img[N * M], 64);

    // ��ʼ��tag
    apriltag_family tf = tag25h9_create();
    tf.init(maxhamming);
    apriltag_detection_info info{nullptr, tagsize, fx, fy, cx, cy};  // ��������tag��λ��

    int32_t pre_time = rt_tick_get();

    for (;;) {
        bool visualize = slave_switch[2].get();  // ���뿪�ؾ����Ƿ���п��ӻ�����Ϊ���ӻ�������ʱ��

        uint8_t* src = mt9v03x_csi_image_take();
        undisort_I(src, img);  // ����ͼ�����
        // rt_memcpy(img, src, N * M);

        detections_t& dets = apriltag_detect(tf, img);  // ���м��

        det_poses_t& det_poses = estimate_poses(info, dets);  // ����tagλ��

        if (visualize) plot_det_poses(img, info, det_poses);  // ���ӻ�

        if (visualize) show_plot_grayscale(img);  // ������ÿ��ӻ��Ͱ�ͼƬ��ʾ����

        mt9v03x_csi_image_release();  // �ͷ�ͼƬ

        int32_t cur_time = rt_tick_get();
        ips114_showint32(188, 0, cur_time - pre_time, 3);  // ��ʾ��ʱ/ms
        pre_time = cur_time;
    }
}

bool apriltagDetectNode() { return FuncThread(apriltagDetectEntry, "apriltagDetect", 4096, 2, 1000); }
