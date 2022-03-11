#include "Thread.h"

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

static void apriltagDetectThreadEntry(void*) {
    using namespace imgProc::apriltag;
    AT_DTCM_SECTION_ALIGN(static uint8_t img[N * M], 64);

    // ��ʼ��tag
    apriltag_family tf = tag25h9_create();
    tf.init(maxhamming);
    // ��������������tag��λ��
    apriltag_detection_info info{nullptr, tagsize, fx, fy, cx, cy};
    apriltag_pose solution;

    int32_t pre_time = rt_tick_get();
    gpio_init(D4, GPI, 0, GPIO_PIN_CONFIG);

    for (;;) {
        uint8_t* src = mt9v03x_csi_image_take();  // ��ȡͼƬ
        undisort_I(src, img);                     // ����ͼ�����

        bool visualize = gpio_get(D4);  // ���뿪�ؾ����Ƿ���п��ӻ�����Ϊ���ӻ�������ʱ��

        // ���м��
        detections_t& dets = apriltag_detect(tf, img);

        // ���������std::forward_list�����
        // ʶ��tag�õ��ڴ���ͳһ����ģ��������ﴦ����Ҳ����Ҫ�ͷ���Щָ��
        for (apriltag_detection* det_p : dets) {  // �������
            apriltag_detection& det = *det_p;
            info.det = det_p;

            // ����tag��λ�ˣ������������solution
            estimate_pose_for_tag_homography(info, solution);

            if (visualize) {                          // ������п��ӻ�������ԭͼ�Ͻ����޸�
                plot_tag_det(img, det);               // ����tag�߿��id
                plot_pose_axis(img, info, solution);  // ����tag������ϵ��RGB�ֱ�Ϊxyz��
                // plot_pose_cube(img, info, solution); // ����tag��3d����
            }

            // TODO ���д���
        }
        if (visualize) show_plot_grayscale(img);  // ������ÿ��ӻ��Ͱ�ͼƬ��ʾ����
        mt9v03x_csi_image_release();              // �ͷ�ͼƬ
        int32_t cur_time = rt_tick_get();
        ips114_showint32(188, 0, cur_time - pre_time, 3);  // ��ʾ��ʱ/ms
        pre_time = cur_time;
    }
}

rtthread::Thread apriltagDetectThread(apriltagDetectThreadEntry, NULL, 4096, 1, 1000, "apriltagDetectThread");