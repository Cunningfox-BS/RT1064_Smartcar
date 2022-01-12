#ifndef _MQueue_hpp
#define _MQueue_hpp

// �о�RTT�ٷ������ʵ���е����⣬�����Լ�дһ�����󲿷ִ���Ӧ�ò��
#include <rtthread.h>

class MQueue {
 private:
    rt_messagequeue mID;
    void* Pool;

 protected:
    void* tmp;
    const size_t Data_sz;

 public:
    MQueue(size_t data_sz, size_t data_cnt, const char* name = "mq") : Data_sz(data_sz) {
        size_t pool_sz = (sizeof(void*) + RT_ALIGN(data_sz, RT_ALIGN_SIZE)) * data_cnt;
        Pool = rt_malloc(pool_sz);
        tmp = rt_malloc(data_sz);
        rt_mq_init(&mID, name, Pool, Data_sz, pool_sz, RT_IPC_FLAG_FIFO);
    }
    virtual ~MQueue() {
        rt_free(Pool);
        rt_free(tmp);
        rt_mq_detach(&mID);
    }

    /**
     * @brief ���β��������
     *
     * @param data          Ҫ���������
     * @param timeout_tick  ��ʱʱ�䣬Ϊ0������
     * @return true         ʧ�ܣ��������������ʱ
     * @return false        �ɹ�
     */
    bool pushback(const void* data, int32_t timeout_tick = 0) {
        return rt_mq_send_wait(&mID, data, Data_sz, timeout_tick) != RT_EOK;
    };

    /**
     * @brief ����ײ������ݣ���֪��Ϊɶ�ٷ����ﲻҲŪ���趨��ʱ�ģ�
     *
     * @param data      Ҫ���������
     * @return true     ʧ�ܣ����������
     * @return false    �ɹ�
     */
    bool pushfront(const void* data) { return rt_mq_urgent(&mID, data, Data_sz) != RT_EOK; };

    /**
     * @brief ȡ����������
     *
     * @param data          ������ԭ��ȡ������
     * @param timeout_tick  ��ʱʱ�䣬С��0�����õȴ�������0����������
     * @return true         ʧ�ܣ��糬ʱ
     * @return false        �ɹ�
     */
    bool popfront(void* data, int32_t timeout_tick = -1) {
        return rt_mq_recv(&mID, data, Data_sz, (timeout_tick < 0) ? -1 : timeout_tick) != RT_EOK;
    };

    /**
     * @brief �������ط������ݣ�������������򵯳����ȷ��������
     *
     * @param data Ҫ���������
     */
    void put(const void* data) {
        while (pushback(data)) popfront(tmp, 0);
    };

    /**
     * @brief �����ػ�ȡ����
     *
     * @param data ������ԭ��ȡ������
     */
    void get(void* data) { popfront(data); };

    /**
     * @brief ��ȡ������ռ�ֽ���
     *
     * @return size_t ������ռ�ֽ���
     */
    size_t data_size() const { return Data_sz; }
};

#endif  // _MQueue_hpp