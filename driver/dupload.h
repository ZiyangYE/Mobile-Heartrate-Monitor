#ifndef __DUPLOAD_H__
#define __DUPLOAD_H__
#include <rtthread.h>
#include <rtdevice.h>
#include "at.h"

//初始化air720模组
rt_err_t up_init(void);

//发送心率信息
rt_err_t send_info(uint8_t devNum, uint8_t heartRate);

#endif
