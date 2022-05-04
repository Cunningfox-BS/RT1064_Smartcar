#ifndef _parameters_hpp
#define _parameters_hpp

#include <cstdint>

namespace Param {

constexpr int32_t MotorControlPeriod = 1;  // ������Ƶ�����, ��λ��tick, �൱��ms

namespace Motor {

constexpr int32_t deadzone_thresh = 200;  // ���pwm���ڴ�ֵʱ������pwm����ȥ

namespace L1 {
constexpr float wc = 20;
constexpr float wo = 40;
constexpr float b0 = 0.004;
constexpr int32_t deadzone = 2000;
}  // namespace L1

namespace L2 {
constexpr float wc = 20;
constexpr float wo = 40;
constexpr float b0 = 0.004;
constexpr int32_t deadzone = 2000;
}  // namespace L2

namespace R1 {
constexpr float wc = 20;
constexpr float wo = 40;
constexpr float b0 = 0.004;
constexpr int32_t deadzone = 2000;
}  // namespace R1

namespace R2 {
constexpr float wc = 20;
constexpr float wo = 40;
constexpr float b0 = 0.004;
constexpr int32_t deadzone = 2000;
}  // namespace R2

}  // namespace Motor

}  // namespace Param

#endif  // _parameters_hpp