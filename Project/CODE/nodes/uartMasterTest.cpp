#include "utils/FuncThread.hpp"
//
#include "devices.hpp"

static rt_mutex_t io_lock;

static void uart2RecvEntry() {
    uint8_t ch;
    for (;;)
        if (uart2.getchar(ch)) {
            rt_mutex_take(io_lock, RT_WAITING_FOREVER);
            rt_kprintf("2: %c\r\n", ch);
            rt_mutex_release(io_lock);
        }
}

static void uart3RecvEntry() {
    uint8_t ch;
    for (;;)
        if (uart3.getchar(ch)) {
            rt_mutex_take(io_lock, RT_WAITING_FOREVER);
            rt_kprintf("3: %c\r\n", ch);
            rt_mutex_release(io_lock);
        }
}

static void uart4RecvEntry() {
    uint8_t ch;
    for (;;)
        if (uart4.getchar(ch)) {
            rt_mutex_take(io_lock, RT_WAITING_FOREVER);
            rt_kprintf("4: %c\r\n", ch);
            rt_mutex_release(io_lock);
        }
}

static void uart5RecvEntry() {
    uint8_t ch;
    for (;;)
        if (uart5.getchar(ch)) {
            rt_mutex_take(io_lock, RT_WAITING_FOREVER);
            rt_kprintf("5: %c\r\n", ch);
            rt_mutex_release(io_lock);
        }
}

static void wirelessEntry() {
    static SerialIO::TxUtil<float, 1> uart2tx("uart2tx"), uart3tx("uart3tx"), uart4tx("uart4tx"), uart5tx("uart5tx");
    for (;;) {
        wireless.waitHeader();
        uint8_t id;
        float t;
        if (!wireless.getchar(id)) continue;  // ��������id, �Ͳ��ö���дЭ����
        if (!wireless.getchar(id)) continue;
        if (!wireless.getData<float>(t)) continue;
        ips.printf("%f\n", t);
        if (uart2tx.txFinished()) {
            uart2tx.setAll(t);
            uart2.send(uart2tx);
        }
        if (uart3tx.txFinished()) {
            uart3tx.setAll(t);
            uart3.send(uart3tx);
        }
        if (uart4tx.txFinished()) {
            uart4tx.setAll(t);
            uart4.send(uart4tx);
        }
        if (uart5tx.txFinished()) {
            uart5tx.setAll(t);
            uart5.send(uart5tx);
        }
    }
}

bool uartMasterTest() {
    io_lock = rt_mutex_create("io_lock", RT_IPC_FLAG_FIFO);
    if (io_lock == RT_NULL) return false;
    return FuncThread(wirelessEntry, "wireless", 1024, 0) &&   //
           FuncThread(uart2RecvEntry, "uart2Recv", 512, 1) &&  //
           FuncThread(uart3RecvEntry, "uart3Recv", 512, 1) &&  //
           FuncThread(uart4RecvEntry, "uart4Recv", 512, 1) &&  //
           FuncThread(uart5RecvEntry, "uart5Recv", 512, 1);
}