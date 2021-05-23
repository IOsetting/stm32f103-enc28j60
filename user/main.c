/*
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
*/
#include <stdio.h>
#include "stm32f10x_conf.h"
#include "tim.h"
#include "uart.h"

#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"
#include "spi.h"
#include "httpd.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

const u16 IP_ADDR[4] = {192,168,  6,203};
const u16 IP_MASK[4] = {255,255,255,  0};
const u16 GW_ADDR[4] = {192,168,  6,  1};

/*******************************************************************************
*	函数名：UipPro
*	输  入:
*	输  出:
*	功能说明：uip协议栈的实现入口，必须被轮询处理。未用中断模式
*/
void UipPro(void)
{
	uint8_t i;
	static struct timer periodic_timer, arp_timer;
	static char timer_ok = 0;	/* fd*/

	/* 创建2个定时器，只用执行1次 */
	if (timer_ok == 0)
	{
		timer_ok = 1;
		timer_set(&periodic_timer, CLOCK_SECOND / 2);  /* 创建1个0.5秒的定时器 */
		timer_set(&arp_timer, CLOCK_SECOND * 10);	   /* 创建1个10秒的定时器 */
	}

	/*
		从网络设备读取一个IP包,返回数据长度 (非阻塞)
		这个地方没有使用DM9000AEP的中断功能，采用的是查询方式
	*/
	uip_len = tapdev_read();	/* uip_len 是在uip中定义的全局变量 */
	if(uip_len > 0)
	{
		/* 处理IP数据包(只有校验通过的IP包才会被接收) */
		if(BUF->type == htons(UIP_ETHTYPE_IP))
		{
			uip_arp_ipin();
			uip_input();
			/*
				当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
				需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
			*/
			if (uip_len > 0)
			{
				uip_arp_out();
				tapdev_send();
			}
		}
		/* 处理arp报文 */
		else if (BUF->type == htons(UIP_ETHTYPE_ARP))
		{
			uip_arp_arpin();
			/*
				当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
				需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
			*/
			if (uip_len > 0)
			{
				tapdev_send();
			}
		}
	}
	else if(timer_expired(&periodic_timer))	/* 0.5秒定时器超时 */
	{
		timer_reset(&periodic_timer);	/* 复位0.5秒定时器 */

		/* 轮流处理每个TCP连接, UIP_CONNS缺省是10个 */
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);	/* 处理TCP通信事件 */
			/*
				当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
				需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
			*/
			if(uip_len > 0)
			{
				uip_arp_out();
				tapdev_send();
			}
		}

	#if UIP_UDP
		/* 轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个 */
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);	/*处理UDP通信事件 */
			/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
			if(uip_len > 0)
			{
			uip_arp_out();
			tapdev_send();
			}
		}
	#endif /* UIP_UDP */

		/* 每隔10秒调用1次ARP定时器函数 */
		if (timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3; //LED2, LED3
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOB, GPIO_Pin_8);
  Systick_Delay_ms(100);
  GPIO_SetBits(GPIOB, GPIO_Pin_8 );		 	 	   //复位ENC28J60
  Systick_Delay_ms(100);
}

int main(void)
{
  SystemInit();
  Systick_Init();
  USART1_Init(); // for printf() logging
  
	GPIO_Configuration();

  SPI1_Init();

  tapdev_init();

	uip_init();

  uip_ipaddr_t ipaddr;
  // uip_hostaddr (host address, chip address)
	printf("IP Address: %d.%d.%d.%d\n\r", IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3]);
	uip_ipaddr(ipaddr, IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3]);
	uip_sethostaddr(ipaddr);

  // uip_netmask: IP network mask
	printf("IP Network Mask: %d.%d.%d.%d\n\r", IP_MASK[0], IP_MASK[1], IP_MASK[2], IP_MASK[3]);
	uip_ipaddr(ipaddr, IP_MASK[0], IP_MASK[1], IP_MASK[2], IP_MASK[3]);
	uip_setnetmask(ipaddr);

  // uip_draddr: default route (default gateway)
  printf("GW Address: %d.%d.%d.%d\n\r", GW_ADDR[0], GW_ADDR[1], GW_ADDR[2], GW_ADDR[3]);
	uip_ipaddr(ipaddr, GW_ADDR[0], GW_ADDR[1], GW_ADDR[2], GW_ADDR[3]);
	uip_setdraddr(ipaddr);

  uip_listen(HTONS(1200));
  httpd_init();
  while (1) {
    UipPro();
  }
}
