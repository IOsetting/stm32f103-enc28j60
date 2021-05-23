# stm32f103-enc28j60
A working demo of ENC28J60 Ethernet module running on STM32F103C8T6.  
This is one of the few working demos on STM32F103C8T6.

# Components Required

* STM32F103C8 board
* ENC28J60 Ethernet RJ45 module
* USB2TTL: PL2303 or CH340 or FT232 or CP2102
* ST-Link or J-LInk
* Router/Switch Hub for Ethernet connection

# Circuit Diagram and connections

Refer below table to connect ESP-01S pins with STM32 pins:

| ENC28J60  | STM32    | USB2TTL      |
| --------- | -------- | ------------ |
| GND       | GND      | GND          |
| VCC       | 3.3V     |              |
| RESET     | PB8      |              |
| CS        | PA4      |              |
| SCK       | PA5      |              |
| SI        | PA7      |              |
| SO        | PA6      |              |
| WCL       |          |              |
| INT       |          |              |
| CLKOUT    |          |              |
|           | PA9      | RX           |
|           | PA10     | TX           |


# Working & Code Explanation

Use ST-Link to upload the code to STM32.  

Connect ST-Link with STM32F103C8T6 Development Board
```
G   -- GND
CLK -- SWCLK
IO  -- SWDIO
V3  -- 3.3V
```

## Configuration

### MAC address

Usually you don't need to change it. The MAC address is configurated in enc28j60_uip.c
```c
static unsigned char mymac[6] = {0x04,0x02,0x35,0x00,0x00,0x01};
```

### IP Addresses

Edit main.c, change the following lines to your IP addresses
```c
const u16 IP_ADDR[4] = {192,168,  6,203};
const u16 IP_MASK[4] = {255,255,255,  0};
const u16 GW_ADDR[4] = {192,168,  6,  1};
```

### HTML Pages

The HTML content are defined in httpd-fsdata.c as a linked array, the entry variable is `HTTPD_FS_ROOT`.

### Printf Output 

Add `PRINT_TO_UART` to preprocessor symbols, this will enable the printf-redirect-to-USART1.

## Compile 

Include Paths
```
..\libraries\CMSIS\CM3\CoreSupport;..\libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x;..\libraries\CMSIS\CM3\DeviceSupport\ST\STM32F10x\startup\arm;..\libraries\STM32F10x_StdPeriph_Driver\inc;..\libraries\uIP-1.0\uip;..\libraries\uIP-1.0\apps\webserver;..\libraries\Hardware;..\user
```
Compile Control String
```bash
--c99 --gnu -c --cpu Cortex-M3 -D__MICROLIB -g -O0 --apcs=interwork --split_sections -I ../libraries/CMSIS/CM3/CoreSupport -I ../libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x -I ../libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/arm -I ../libraries/STM32F10x_StdPeriph_Driver/inc -I ../libraries/uIP-1.0/uip -I ../libraries/uIP-1.0/apps/webserver -I ../libraries/Hardware -I ../user
-I./RTE/_stm32f103_enc28j60
-IC:/Keil_v5/ARM/PACK/Keil/STM32F1xx_DFP/2.3.0/Device/Include
-IC:/Keil_v5/ARM/CMSIS/Include
-D__UVISION_VERSION="525" -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DPRINT_TO_UART
-o .\Objects\*.o --omf_browse .\Objects\*.crf --depend .\Objects\*.d
```

## Test

* Ping test: Ping the IP address you defined in main.c
* HTTP test: Open a web browser, visit the IP address with `http://` protocol

