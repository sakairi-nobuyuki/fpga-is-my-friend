# Second

## Fixes by ChatGPT

貼ってもらった `xparameters.h` を見る限り、これは **Cora S7 ではなく Cora Z7 / Zynq-7000 系の platform** です。

根拠として、`ps7_cortexa9_0` があり、`XSCUGIC`、`XSCUTIMER`、`PS7_*` が生成されています。つまり **Zynq の ARM Cortex-A9 + SCU Timer + GIC 構成**です。`AXI GPIO` も `0x41200000` に存在しています。 また、GIC は `0xf8f01000`、SCU Timer は `0xf8f00600` にあります。

なので前回の「Cora S7 ならこのコードは違う」は、**今回の `xparameters.h` に関しては該当しません**。これは Zynq 側です。

ただし、今のコードは **Vitis 2023.2 の xparameters 生成形式と合っていない**のが問題です。

---

## まず直すべきところ

あなたの `xparameters.h` には、少なくとも見えている範囲では

```c
XPAR_XSCUTIMER_0_DEVICE_ID
XPAR_PS7_SCUGIC_0_DEVICE_ID
XPAR_GPIO_0_DEVICE_ID
XPAR_SCUTIMER_DEVICE_ID
```

のような旧式の `DEVICE_ID` マクロが出ていません。

代わりにあるのはこれです。

```c
#define XPAR_XGPIO_0_BASEADDR       0x41200000
#define XPAR_XSCUGIC_0_BASEADDR     0xf8f01000
#define XPAR_XSCUTIMER_0_BASEADDR   0xf8f00600
```

したがって、`LookupConfig()` や `Initialize()` に渡す値を **DEVICE_ID ではなく BASEADDR 系**に寄せる必要があります。

---

## 修正ポイント

### GPIO

これはダメです。

```c
Status = XGpio_Initialize(&Gpio, XPAR_XSLCR_0_DEVICE_ID);
```

`XSLCR` は GPIO ではありません。

この環境ではこうしてください。

```c
Status = XGpio_Initialize(&Gpio, XPAR_XGPIO_0_BASEADDR);
```

---

### Timer

これは怪しいです。

```c
ConfigPtr = XScuTimer_LookupConfig(XPAR_SCUTIMER_DEVICE_ID);
```

この環境ではこうです。

```c
ConfigPtr = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_BASEADDR);
```

さらに NULL チェックを入れてください。

```c
if (ConfigPtr == NULL) {
    xil_printf("XScuTimer_LookupConfig failed\r\n");
    return XST_FAILURE;
}
```

---

### GIC

これは間違いです。

```c
ConfigPtr = XScuGic_LookupConfig(XPAR_SCUTIMER_DEVICE_ID);
```

GIC の初期化に Timer の ID を渡しています。

この環境ではこうです。

```c
ConfigPtr = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
```

こちらも NULL チェックしてください。

---

### Timer interrupt ID

あなたの `xparameters.h` では SCU Timer interrupt が

```c
#define XPAR_SCUTIMER_INTERRUPTS 0x13100d
#define XPAR_XSCUTIMER_0_INTERRUPTS 0x13100d
```

になっています。

ただし、`XScuGic_Connect()` に渡す割り込み ID は通常 **GIC interrupt ID** です。SCU private timer は Zynq ではだいたい **29** です。

なので、まずはこう置いてください。

```c
#define SCUTIMER_INTR_ID 29
```

そして

```c
Status = ScuGicInit_Reg(SCUTIMER_INTR_ID, &TimerInstance, TimerCounterHandler);
```

にします。

`0x13100d` をそのまま渡すのはたぶんダメです。これは device-tree 的な packed 表現に近く、GIC の interrupt ID そのものではないです。

---

## LED が変化しない理由も別にあります

今の割り込みハンドラでは、`cnt` を増やしているだけで LED に書いていません。

```c
if (cnt++ > 9) cnt = 0;
```

なので、仮に割り込みが動いていても LED は変わりません。

こうしてください。

```c
void TimerCounterHandler(void *CallBackRef)
{
    static int cnt = 0;
    XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;

    if (XScuTimer_IsExpired(TimerInstancePtr)) {
        XScuTimer_ClearInterruptStatus(TimerInstancePtr);

        XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, led_rgb(cnt));

        cnt++;
        if (cnt > 9) {
            cnt = 0;
        }
    }
}
```

---

## 直した版

まずこれで試してください。

```c
#include "xparameters.h"
#include "xgpio.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

#define TIMER_LOAD_VALUE 333333333
#define LED_CHANNEL      1

/* Zynq SCU private timer interrupt ID */
#define SCUTIMER_INTR_ID 29

XGpio Gpio;
XScuTimer TimerInstance;
XScuGic IntcInstance;

int led_rgb(int cnt)
{
    switch (cnt % 5) {
        case 0: return 0x4;
        case 1: return 0x2;
        case 2: return 0x1;
        case 3: return 0x7;
        case 4: return 0x4;
        default: return 0x0;
    }
}

void TimerCounterHandler(void *CallBackRef)
{
    static int cnt = 0;
    XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;

    if (XScuTimer_IsExpired(TimerInstancePtr)) {
        XScuTimer_ClearInterruptStatus(TimerInstancePtr);

        XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, led_rgb(cnt));

        cnt++;
        if (cnt > 9) {
            cnt = 0;
        }
    }
}

int ScuGicInit(void)
{
    int Status;
    XScuGic_Config *ConfigPtr;

    ConfigPtr = XScuGic_LookupConfig(XPAR_XSCUGIC_0_BASEADDR);
    if (ConfigPtr == NULL) {
        xil_printf("XScuGic_LookupConfig failed\r\n");
        return XST_FAILURE;
    }

    Status = XScuGic_CfgInitialize(
        &IntcInstance,
        ConfigPtr,
        ConfigPtr->CpuBaseAddress
    );
    if (Status != XST_SUCCESS) {
        xil_printf("XScuGic_CfgInitialize failed\r\n");
        return XST_FAILURE;
    }

    Xil_ExceptionInit();

    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_INT,
        (Xil_ExceptionHandler) XScuGic_DeviceInterruptHandler,
        &IntcInstance
    );

    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

int ScuGicInit_Reg(u32 Int_Id, void *InstancePtr, void *IntHandler)
{
    int Status;

    Status = XScuGic_Connect(
        &IntcInstance,
        Int_Id,
        (Xil_ExceptionHandler) IntHandler,
        InstancePtr
    );

    if (Status != XST_SUCCESS) {
        xil_printf("XScuGic_Connect failed\r\n");
        return XST_FAILURE;
    }

    XScuGic_Enable(&IntcInstance, Int_Id);

    return XST_SUCCESS;
}

int main()
{
    int Status;
    XScuTimer_Config *ConfigPtr;

    xil_printf("Timer Interrupt Test\r\n\r\n");

    xil_printf("Initializing GPIO\r\n");

    Status = XGpio_Initialize(&Gpio, XPAR_XGPIO_0_BASEADDR);
    if (Status != XST_SUCCESS) {
        xil_printf("GPIO init failed\r\n");
        return XST_FAILURE;
    }

    XGpio_SetDataDirection(&Gpio, LED_CHANNEL, 0x0);
    XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, led_rgb(0));

    xil_printf("Initialized GPIO\r\n\r\n");

    xil_printf("Initializing timer\r\n");

    ConfigPtr = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_BASEADDR);
    if (ConfigPtr == NULL) {
        xil_printf("XScuTimer_LookupConfig failed\r\n");
        return XST_FAILURE;
    }

    Status = XScuTimer_CfgInitialize(
        &TimerInstance,
        ConfigPtr,
        ConfigPtr->BaseAddr
    );

    if (Status != XST_SUCCESS) {
        xil_printf("Timer init failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("Initialized timer\r\n\r\n");

    xil_printf("Initializing interrupt\r\n");

    Status = ScuGicInit();
    if (Status != XST_SUCCESS) {
        xil_printf("ScuGicInit failed\r\n");
        return XST_FAILURE;
    }

    Status = ScuGicInit_Reg(SCUTIMER_INTR_ID, &TimerInstance, TimerCounterHandler);
    if (Status != XST_SUCCESS) {
        xil_printf("ScuGicInit_Reg failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("Initialized interrupt\r\n\r\n");

    xil_printf("Starting timer\r\n");

    XScuTimer_EnableAutoReload(&TimerInstance);
    XScuTimer_LoadTimer(&TimerInstance, TIMER_LOAD_VALUE);
    XScuTimer_EnableInterrupt(&TimerInstance);
    XScuTimer_Start(&TimerInstance);

    xil_printf("Started timer\r\n");

    while (1);

    return 0;
}
```

---

## 重要な注意

`TIMER_LOAD_VALUE 333333333` はかなり大きいです。クロックによっては LED が変わるまで数秒以上かかります。最初の動作確認では小さくしてください。

例えば：

```c
#define TIMER_LOAD_VALUE 33333333
```

またはもっと確認しやすく：

```c
#define TIMER_LOAD_VALUE 10000000
```

にして、LED が変わるか見た方がよいです。

---

## 今回の本命原因

今回の止まり方からすると、本命はこれです。

```c
ConfigPtr = XScuTimer_LookupConfig(XPAR_SCUTIMER_DEVICE_ID);
```

この `XPAR_SCUTIMER_DEVICE_ID` があなたの BSP の形式と合っていません。
`xparameters.h` では `XPAR_XSCUTIMER_0_BASEADDR 0xf8f00600` が生成されているので、そちらを使うべきです。

また GPIO も `XPAR_XSLCR_0_DEVICE_ID` ではなく、AXI GPIO の `XPAR_XGPIO_0_BASEADDR 0x41200000` を使うべきです。
