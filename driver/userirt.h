#ifndef __USERIRT_H__
#define __USERIRT_H__
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_gpio.h"

//初始化中断
rt_err_t irt_init(void);

//中断处理函数
void irt_handler(void *args);

#endif
