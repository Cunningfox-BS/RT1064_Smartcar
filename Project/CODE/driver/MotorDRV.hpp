#ifndef _MotorDRV_hpp
#define _MotorDRV_hpp

extern "C" {
#include "zf_gpio.h"
#include "zf_pwm.h"
}

class MotorDRV {
 private:
    const PIN_enum DIR_pin;
    const PWMCH_enum PWM_ch;
    const bool invert;

 public:
    MotorDRV(PIN_enum dir_pin, PWMCH_enum pwm_ch, bool inverted);
    void init();

    /**
     * @brief ���������PWM����
     *
     * @param duty Ҫ���õ�pwmֵ, ����Ǹ�����ᷴ��; ����ֵ���Ϊ50000(fsl_pwm.h -> PWM_DUTY_MAX)
     */
    void setPWM(int32 duty);
};

#endif  // _MotorDRV_hpp