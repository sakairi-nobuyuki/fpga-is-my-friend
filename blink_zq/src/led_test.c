#include "xparameters.h"
#include "xil_printf.h"

#define LED      *((volatile unsigned int*) (XPAR_GPIO_0_BASEADDR + 0x00))
#define LED_ctrl *((volatile unsigned int*) (XPAR_GPIO_0_BASEADDR + 0x04))

int main()
{
    int i, j;

    LED_ctrl = 0x0;
    xil_printf("hoge FPGA dayo\n");
    while (1) {
        for (i = 0; i < 5; i++) {
            xil_printf("i=%d\r\n", i);
            switch (i)
            {
                case 0: LED = 0x4; break;
                case 1: LED = 0x2; break;
                case 2: LED = 0x1; break;
                case 3: LED = 0x7; break;
                case 4: LED = 0x4; break;
                default: LED =0x0;
            }
        }
        for (j = 0; j=400000000; j++);
    }
    return 0;

}
