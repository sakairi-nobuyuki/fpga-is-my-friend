#include "xparameters.h"
#include "xgpio.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

#define TIMER_LOAD_VALUE 333333333
#define LED_CHANNEL      1
#define SCUTIMER_INTR_ID 29

/* GPIO related global variables */
XGpio Gpio;
XScuTimer TimerInstance;
XScuGic IntcInstance;


/* LED blink pattern */
int led_rgb(int cnt)
{
    int led;
    switch (cnt % 5) {
        case 0: 
            led = 0x4; 
            xil_printf("RED\n");
            break;
        case 1: 
            led = 0x2; 
            xil_printf("GREEN\n");
            break;
        case 2: 
            led = 0x1; 
            xil_printf("BLUE\n");
            break;
        case 3: 
            led = 0x7; 
            xil_printf("WHITE\n");
            break;
        case 4: 
            led = 0x4; 
            xil_printf("BLANK\n");
            break;
        default: led =0x0;
    }
    return led;
}


/* Timer interrupt function */
void TimerCounterHandler(void *CallBackRef) 
{
    volatile static int cnt;
    XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;

    if (XScuTimer_IsExpired(TimerInstancePtr)) { /* check if the timer pointer is effective */
        XScuTimer_ClearInterruptStatus(TimerInstancePtr);
    }

    cnt++;
    if (cnt > 9) cnt = 0; /* Clear if counter exceeds 9 */

}


/* Interrupt driver initialization */
int ScuGicInit(void) 
{
    int Status;
    XScuGic_Config *ConfigPtr;
    
//    ConfigPtr = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID); // XPAR_SCUTIMER_DEVICE_ID
//    ConfigPtr = XScuGic_LookupConfig(XPAR_SCUTIMER_DEVICE_ID); // XPAR_SCUTIMER_DEVICE_ID
// [ERROR] /home/nsakairi/cora_z7_ws/second/second.vitis/second/src/second.c:58:38: error: 
// 'XPAR_XSCUGIC_0_BASEDDR' undeclared (first use in this function); did you mean 'XPAR_XSCUGIC_0_BASEADDR'?
    ConfigPtr = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR); // XPAR_SCUTIMER_DEVICE_ID
    Status = XScuGic_CfgInitialize(&IntcInstance, ConfigPtr, ConfigPtr->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("XScuGic_CfgInitialize failed\n\n");
        return XST_FAILURE;
    }

    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_DeviceInterruptHandler, &IntcInstance
    );
    Xil_ExceptionEnable();
    return XST_SUCCESS;
}

int ScuGicInit_Reg(u32 Int_Id, void *InstancePtr, void *IntHandler)
{
    /**
     * @param void *IntHandler Hand over user defined interrupt function
     * @brief Registre interrupt function
     *  */    

    int Status;
    Status = XScuGic_Connect(&IntcInstance, Int_Id, (Xil_ExceptionHandler) IntHandler, (void *) InstancePtr);
    if (Status != XST_SUCCESS) return XST_FAILURE;
    XScuGic_Enable(&IntcInstance, Int_Id);
    return XST_SUCCESS;
}

int main ()
{
    int Status;
    XScuTimer_Config *ConfigPtr;

    xil_printf("Timer Interrupt Test\n\n");

    /* Initialize GPIO */
    xil_printf("Initializing GPIO\n");
    //    Status = XGpio_Initialize(&Gpio, XPAR_GPIO_0_DEVICE_ID);
    Status = XGpio_Initialize(&Gpio, XPAR_XGPIO_0_BASEADDR);
    if (Status != XST_SUCCESS) return XST_FAILURE;
    XGpio_SetDataDirection(&Gpio, LED_CHANNEL, 0);
    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, led_rgb(0));
    xil_printf("Initialized GPIO\n\n");


    /* Initialize Timer Driver */
    xil_printf("Initializing timer\n");
//    ConfigPtr = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_DEVICE_ID);
    ConfigPtr = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_BASEADDR);
    Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        xil_printf("Timer driver XScuTimer_LookupConfig failed\n\n");
        return XST_FAILURE;
    }
    
    xil_printf("Initialized timer\n\n");

    /* Initialize interrupt related and interrupt handler */
    xil_printf("Initializing interrupt\n");
    Status = ScuGicInit();
    if (Status != XST_SUCCESS) return XST_FAILURE;
//    Status = ScuGicInit_Reg(XPAR_SCUTIMER_INTR, &TimerInstance, TimerCounterHandler);
    Status = ScuGicInit_Reg(SCUTIMER_INTR_ID, &TimerInstance, TimerCounterHandler);
    if (Status != XST_SUCCESS) return XST_FAILURE;
    xil_printf("Initialized interrupt\n\n");

    /* Initialize timer and start */
    xil_printf("Initializing timer\n");
    XScuTimer_EnableAutoReload(&TimerInstance);
    XScuTimer_LoadTimer(&TimerInstance, TIMER_LOAD_VALUE);
    XScuTimer_EnableInterrupt(&TimerInstance);
    XScuTimer_Start(&TimerInstance);
    xil_printf("Initialized timer\n");

    while(1);
    return 0;
}

