#ifndef _MQueue_hpp
#define _MQueue_hpp

// �о�RTT�ٷ������ʵ���е����⣬�����Լ�дһ�����󲿷ִ���Ӧ�ò��
#include <rtthread.h>

template <typename T, int N> class MQueue {
 private:
    rt_messagequeue mID;
    char mPool[(sizeof(void*) + RT_ALIGN(sizeof(T), RT_ALIGN_SIZE)) * N];

 public:
    MQueue(const char* name = "mq") { rt_mq_init(&mID, name, mPool, sizeof(T), sizeof(mPool), RT_IPC_FLAG_FIFO); };
    ~MQueue() { rt_mq_detach(&mID); };

    /**
     * @brief ���β��������
     *
     * @param data          Ҫ���������
     * @param timeout_tick  ��ʱʱ�䣬Ϊ0������
     * @return true         ʧ�ܣ��������������ʱ
     * @return false        �ɹ�
     */
    bool pushback(const T& data, int32_t timeout_tick = 0) {
        return rt_mq_send_wait(&mID, &data, sizeof(data), timeout_tick) != RT_EOK;
    };

    /**
     * @brief ����ײ������ݣ���֪��Ϊɶ�ٷ����ﲻҲŪ���趨��ʱ�ģ�
     *
     * @param data      Ҫ���������
     * @return true     ʧ�ܣ����������
     * @return false    �ɹ�
     */
    bool pushfront(const T& data) { return rt_mq_urgent(&mID, &data, sizeof(data)) != RT_EOK; };

    /**
     * @brief ȡ����������
     *
     * @param data          ������ԭ��ȡ������
     * @param timeout_tick  ��ʱʱ�䣬С��0�����õȴ�������0����������
     * @return true         ʧ�ܣ��糬ʱ
     * @return false        �ɹ�
     */
    bool popfront(T& data, int32_t timeout_tick = -1) {
        return rt_mq_recv(&mID, &data, sizeof(data), (timeout_tick < 0) ? -1 : timeout_tick) != RT_EOK;
    };

    /**
     * @brief �������ط������ݣ�������������򵯳����ȷ��������
     *
     * @param data Ҫ���������
     */
    void put(const T& data) {
        while (pushback(data)) {
            T tmp;
            popfront(tmp, 0);
        }
    };

    /**
     * @brief �����ػ�ȡ����
     *
     * @param data ������ԭ��ȡ������
     */
    void get(T& data) { popfront(data); };
};

template <typename T> class SensorDataBuf {
 private:
    MQueue<T, 1> mq;

 public:
    void put(const T& data) {
        while (mq.pushback(data)) {
            T tmp;
            mq.popfront(tmp, 0);
        }
    }
    void get(T& data) { mq.popfront(data); }
};

#endif  // _MQueue_hpp