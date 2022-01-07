extern "C" {
#include "headfile.h"
int main(void);
}

#include "SerialIO.hpp"

SerialIO wireless;

double aa, bb, cc;

int main(void) {
    //�˴���д�û�����(���磺�����ʼ�������)

    gpio_init(B9, GPO, 0, GPIO_PIN_CONFIG);

    wireless.init("wireless", WIRELESS_UART, WIRELESS_UART_BAUD, WIRELESS_UART_TX, WIRELESS_UART_RX);

    EnableGlobalIRQ(0);

    while (1) {
        //�˴���д��Ҫѭ��ִ�еĴ���
        wireless.waitHeader();
        if (wireless.readD(aa, bb, cc)) {
            rt_kprintf("%d %d %d\n", (int)aa, (int)bb, (int)cc);
            wireless.sendHeader();
            wireless.writeD(aa, bb, cc);
        } else
            rt_kputs("error\n");
        gpio_toggle(B9);
    }
}
