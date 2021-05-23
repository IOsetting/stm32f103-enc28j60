#include "tapdev.h"
#include "uip.h"
#include "enc28j60_uip.h"

/* Init device, set IP address */
void tapdev_init(void)
{
  etherdev_init();
}

/* Read one packet */
uint16_t tapdev_read(void)
{	
  return etherdev_read(uip_buf);
}

/* Send one packet */
void tapdev_send(void)
{
  etherdev_send(uip_buf, uip_len);
}
