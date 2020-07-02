#ifndef __HRATE_H__
#define __HRATE_H__

#include "max30102.h"

//初始化I2C及30102
rt_err_t hrate_init(void);

//获取红外和红光的读数
rt_err_t get_IRV(rt_uint32_t *pun_red_led, rt_uint32_t *pun_ir_led);

#endif
