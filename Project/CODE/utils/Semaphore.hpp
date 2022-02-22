#ifndef _Semaphore_hpp
#define _Semaphore_hpp

#include <rtthread.h>

class Semaphore {
    rt_semaphore mID;
    bool initialized = false;

 public:
    Semaphore() = default;
    // flag����ȡ RT_IPC_FLAG_FIFO(������, ��ʵʱ) �� RT_IPC_FLAG_PRIO(ʵʱ)
    Semaphore(const char *name, int32_t count, rt_uint8_t flag = RT_IPC_FLAG_PRIO) { init(name, count, flag); }
    void init(const char *name, int32_t count, rt_uint8_t flag = RT_IPC_FLAG_PRIO) {
        rt_sem_init(&mID, name, count, flag);
        initialized = true;
    }
    ~Semaphore() {
        if (initialized) rt_sem_detach(&mID);
    }
    // С��0���õȴ�, ����0��������, ����0�ȴ�ָ��ʱ��
    bool wait(int32_t timeout_ms = -1) {
        rt_int32_t tick = (timeout_ms < 0) ? -1 : rt_tick_from_millisecond(timeout_ms);
        return rt_sem_take(&mID, tick) == RT_EOK;
    }
    void release(void) { rt_sem_release(&mID); }
};

#endif  //_Semaphore_hpp