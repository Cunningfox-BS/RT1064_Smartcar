#ifndef _fieldParam_hpp
#define _fieldParam_hpp

#include <cstdint>

#include "pose_kalman/utils.hpp"
using pose_kalman::PI, pose_kalman::PI_2;

// ��λ������

// A4ֽ�ϱ߽�Ŀ��
constexpr float borderWidth = 7;
constexpr float borderHeight = 5;

// ����һ��Ĵ�С�����ڶ���A4ֽʶ����
constexpr bool squareAlign = true;  // �Ƿ��A4ʶ�������ж���
constexpr float squareSize = 0.2;

// ʵ�������Ŀ��
constexpr float fieldWidth = 7;
constexpr float fieldHeight = 5;

// ����ʱ�߽���ľ���
constexpr float carryExtendPadding = 0.0;  // ����
constexpr float carrySidePadding = 0.55;   // ���������м�

// ������rect����߽����С����
constexpr bool useRectPadding = false;
constexpr float rectPadding = 0.1;

// ���ڳ�����ʹrect��Ч����С����
constexpr bool useRectBasePadding = false;
constexpr float rectBasePadding = 0.1;

// rectʶ����������ƫ�ƾ���
constexpr float rectMaxDistError = 0.5;

// ��ʼλ��
constexpr double initial_position[3]{0.210 / 2, -(0.282 / 2 + 20e-2), -PI_2};
constexpr double out_garage_y = 0.282 / 2 + 5e-2;

#endif  // _fieldParam_hpp