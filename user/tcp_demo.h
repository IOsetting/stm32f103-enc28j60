#ifndef __TCP_DEMO_H__
#define __TCP_DEMO_H__

/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */

#include "uipopt.h"


/* 通信程序状态字(用户可以自己定义) */
enum
{
  STATE_CMD		  = 0,	/* 命令接收状态 */
  STATE_TX_TEST	= 1,	/* 连续发送数据包状态(速度测试) */
  STATE_RX_TEST	= 2		/* 连续接收数据包状态(速度测试) */
};

/*
	定义 uip_tcp_appstate_t 数据类型，用户可以添加应用程序需要用到
	成员变量。不要更改结构体类型的名字，因为这个类型名会被uip引用。

	uip.h 中定义的 	struct uip_conn  结构体中引用了 uip_tcp_appstate_t
*/
struct tcp_demo_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};

typedef struct tcp_demo_appstate uip_tcp_appstate_t;

/* 定义应用程序回调函数 */
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_server_appcall
#endif

void tcp_demo_appcall(void);

#endif
