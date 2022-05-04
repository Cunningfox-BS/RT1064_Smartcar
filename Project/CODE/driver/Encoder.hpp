#ifndef _Encoder_hpp
#define _Encoder_hpp

#include "QTimer.hpp"
#include "Systick.hpp"
#include "parameters.hpp"

class Encoder {
    const QTimer& _qtimer;
    using qtimer_data_t = decltype(_qtimer.get());
    qtimer_data_t _last;
    uint64_t _last_time;
    float mps;  // m/s ��ÿ��

    // ���������ת���ת��
    void convert(float delta) {
        constexpr float scalefactor =              // ����ϵ��
            2. * 3.14159265358979323846 / 1024. *  // ������ת1�ܲ���1024������
            30. / 70. *                            // ���ֳ���Ϊ70, ����������Ϊ30
            6.3 / 2. / 100.;                       // ����ֱ��6.3cm
        uint64_t cur_time = systick.get_us();
        int64_t dt = systick.get_diff_us(_last_time, cur_time);
        _last_time = cur_time;
        mps = delta * scalefactor * 1e6 / dt;
    }

 public:
    Encoder(const QTimer& qtimer) : _qtimer(qtimer) {}
    void init() {
        _last = _qtimer.get();
        _last_time = systick.get_us();
        mps = 0;
    }
    void update() {
        qtimer_data_t cur = _qtimer.get();
        qtimer_data_t delta = cur - _last;
        convert(delta);
        _last = cur;
    }
    // m/s ��ÿ��
    float get() const { return mps; }
};

#endif  // _Encoder_hpp