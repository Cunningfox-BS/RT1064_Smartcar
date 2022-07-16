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
constexpr float fieldWidth = 5;
constexpr float fieldHeight = 4;

// ����ʱ�߽���ľ���
constexpr float carryExtendPadding = 0.5;  // ����
constexpr float carrySidePadding = 0.5;    // ���������м�

// ������rect����߽����С����
constexpr float rectPadding = 0.1;

// ���ڳ�����ʹrect��Ч����С����
constexpr bool useRectBasePadding = false;
constexpr float rectBasePadding = 0.1;

// rectʶ����������ƫ�ƾ���
constexpr float rectMaxDistError = 0.5;

// ��ʼλ��
constexpr float garage_position1[2]{0.105, 0.4};
constexpr float garage_position2[2]{0.105, -0.4};
constexpr float initial_position[3]{0.105, -0.141, PI_2};

#endif  // _fieldParam_hpp