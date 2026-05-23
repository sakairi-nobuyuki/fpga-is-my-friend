## Cora Z7S constraints file
## chapter 2
## project: blink

# clock
#set_property PACKAGE_PIN H16       [get_ports { CLK }]
#set_property IOSTANDARD LVCMOS33    [get_ports { CLK }]

#create_clock -add -name sys_clk_pin -period 8.00 -waveform {0 4} [get_ports { CLK }]

# Reset
#set_property -dict { PACKAGE_PIN Y16    IOSTANDARD LVCMOS33} [get_ports { RST }];  #BTN[１]

# RGB
set_property -dict { PACKAGE_PIN L15    IOSTANDARD LVCMOS33} [get_ports { LED_RGB[0] }];  #Blue
set_property -dict { PACKAGE_PIN N15    IOSTANDARD LVCMOS33} [get_ports { LED_RGB[2] }];  #RED
set_property -dict { PACKAGE_PIN G17    IOSTANDARD LVCMOS33} [get_ports { LED_RGB[1] }];  #Green



