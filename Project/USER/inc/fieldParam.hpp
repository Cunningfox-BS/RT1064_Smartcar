#ifndef _fieldParam_hpp
#define _fieldParam_hpp

#include <cstdint>

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
constexpr float carryPadding = 0.3;

// ������rect����߽����С����
constexpr float rectPadding = 0.1;

// ���ڳ�����ʹrect��Ч����С����
constexpr float rectBasePadding = 0.1;

// rectʶ����������ƫ�ƾ���
constexpr float rectMaxDistErrorSquared = 0.5 * 0.5;

#endif  // _fieldParam_hpp