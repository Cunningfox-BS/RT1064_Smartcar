#ifndef _nodes_hpp
#define _nodes_hpp

#include "Thread.h"

// ���ߴ��ڽ��ս���, ����ң��
extern rtthread::Thread wirelessThread;

// apriltag���, ��Ҫ��ʼ��mt9v03x
extern rtthread::Thread apriltagDetectThread;

// ��λ����ͼ, ��Ҫ��ʼ��usb_cdc��mt9v03x
extern rtthread::Thread imgUSBXferThread;

// �ð�����ips��Ļ���Ե��
extern rtthread::Thread testMotorThread;

#endif  // _nodes_hpp