#ifndef _nodes_hpp
#define _nodes_hpp

// ���ߴ��ڽ��ս���, ����ң��
bool wirelessNode();

// apriltag���, ��Ҫ��ʼ��mt9v03x
bool apriltagDetectNode();

// ���μ��, ��Ҫ��ʼ��mt9v03x
bool findRectNode();

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

// ���԰���, ��Ҫ�رձ�������ʼ��
bool testKeyNode();

// ���ӻ��������
bool uartMasterTest();
bool uartSlaveTest();

// �������˲���λ
bool poseKalmanNode();

// ���Կ������˲���
bool testPoseKalmanNode();

// ���Ծֲ�·���滮��
bool testLocalPlannerNode();

// ����TSP��
bool testTSPSolverNode();

// ����Canny��Ե���
bool testCannyNode();

#endif  // _nodes_hpp