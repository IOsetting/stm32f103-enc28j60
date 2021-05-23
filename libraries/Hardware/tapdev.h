/*
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: tapdev.h,v 1.1 2002/01/10 06:22:56 adam Exp $
 *
 */

#ifndef __TAPDEV_H__
#define __TAPDEV_H__

#include <stdint.h>

void     tapdev_init(void);
uint16_t tapdev_read(void);
void     tapdev_send(void);

#endif /* __TAPDEV_H__ */
