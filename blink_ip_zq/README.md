# Desgin IP with Vivado template 

See Chapter 7 of the textbook for details.

You can see stdoutput of the CPU of Zynq with 

```bash
screen /dev/ttyUSB* 115200
```

Check the device assignment with, 

```bash
$ ls /dev/ttyUSB*
/dev/ttyUSB0  /dev/ttyUSB1
```

or

```bash
$ lsusb
Bus 002 Device 003: ID 04bb:0176 I-O Data Device, Inc. SSPH-UT
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 001 Device 004: ID 8087:0025 Intel Corp. Wireless-AC 9260 Bluetooth Adapter
Bus 001 Device 003: ID 248a:8367 Maxxter Telink Wireless Receiver
Bus 001 Device 002: ID 0c45:7b36 Microdia Full-Speed Mouse
Bus 001 Device 034: ID 0403:6010 Future Technology Devices International, Ltd FT2232C/D/H Dual UART/FIFO IC
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 004 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 003 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
```

or

```bash
$ ls -l /dev/serial/by-id/
合計 0
lrwxrwxrwx 1 root root 13  6月 28 01:20 usb-Digilent_Digilent_Adept_USB_Device_210370BD055E-if00-port0 -> ../../ttyUSB0
lrwxrwxrwx 1 root root 13  6月 28 01:20 usb-Digilent_Digilent_Adept_USB_Device_210370BD055E-if01-port0 -> ../../ttyUSB1
```
