#ifndef _nodes_hpp
#define _nodes_hpp

// ���ߴ��ڽ��ս���, ����ң��
bool wirelessNode();

// apriltag���, ��Ҫ��ʼ��mt9v03x
bool apriltagDetectNode();

// ��λ����ͼ, ��Ҫ��ʼ��usb_cdc��mt9v03x
bool imgUSBXferNode();

// ʹ��rtthread timer�����������Ե��ת�ٿ���
bool motorControlNode();

// �ð�����ips��Ļ���Ե��
bool testMotorNode();

// ����qtimer
bool testQTimerNode();

// �����λ�����Ե��PWM, ��Ҫ����wirelessNode��motorControlNode
bool testMotorPwmNode();

#endif  // _nodes_hpp