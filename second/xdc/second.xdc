## Cora Z7S constraints file
## chapter 5
## project: second
## reference from 
## - https://digilent.com/reference/programmable-logic/cora-z7/reference-manual
## - https://github.com/motchy869/CompleteWork_of_FPGA_Xilinx_2nd_edition/blob/main/5-4_second/second/xdc/second.xdc

# clock
#set_property PACKAGE_PIN H16       [get_ports { CLK }]
#set_property IOSTANDARD LVCMOS33    [get_ports { CLK }]

#create_clock -add -name sys_clk_pin -period 8.00 -waveform {0 4} [get_ports { CLK }]

# Reset
#set_property -dict { PACKAGE_PIN Y16    IOSTANDARD LVCMOS33} [get_ports { RST }];  #BTN[１]
set_property -dict { PACKAGE_PIN D19    IOSTANDARD LVCMOS33} [get_ports { BTN[1] }]; 
set_property -dict { PACKAGE_PIN D20    IOSTANDARD LVCMOS33} [get_ports { BTN[0] }]; 

# RGB
set_property -dict { PACKAGE_PIN L15    IOSTANDARD LVCMOS33} [get_ports { LED_RGB[0] }];  #Blue
set_property -dict { PACKAGE_PIN N15    IOSTANDARD LVCMOS33} [get_ports { LED_RGB[2] }];  #RED
set_property -dict { PACKAGE_PIN G17    IOSTANDARD LVCMOS33} [get_ports { LED_RGB[1] }];  #Green



