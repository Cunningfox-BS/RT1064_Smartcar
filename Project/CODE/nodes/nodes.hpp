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

// �������˲���λ
bool poseKalmanNode();

// �������մӻ�����
bool uartMasterNode();

// ��������art����
bool uartArtNode();

// ��������ѭ��
bool masterMainLoopNode();

// �ӻ�������������
bool uartSlaveNode();

// �ӻ�����ѭ��
bool slaveMainLoopNode();

// �����ϵͳ������
bool sendTraverseNode();

// ^^^  src  ^^^
// vvv tests vvv

// ���Ե����
bool testMagnetNode();

// ���Ծ��μ��, ��Ҫ��ʼ��mt9v03x
bool testFindRectNode();

// �����������վ��μ����
bool testRectReceiveNode();

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

// ���Կ������˲���
bool testPoseKalmanNode();

// ���Ծֲ�·���滮��
bool testLocalPlannerNode();

// ����TSP��
bool testTSPSolverNode();

// ����Canny��Ե���
bool testCannyNode();

// ����A4ֽ���
bool testA4DetectNode();

// �������˽���A4ֽ�����
bool testA4ReceiveNode();

// ������artͨ��
bool testArtNode();

#endif  // _nodes_hpp