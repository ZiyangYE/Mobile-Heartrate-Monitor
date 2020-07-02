/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-01-08     shelton      first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "drv_gpio.h"

#include "dupload.h"
#include "hrate.h"
#include "userirt.h"
#include "algorithm.h"

rt_sem_t hrate_ready = NULL;
rt_sem_t calc_ready = NULL;
rt_sem_t report_ready = NULL;

//一二缓存 IR/R 数据号
rt_uint32_t buf[2][2][500]; 
//缓存页，通过双页缓存切换，避免写入正在处理的缓存
rt_uint8_t buf_page;

//心率计算结果
rt_uint8_t hrate_rst;

//上传数据线程
static rt_thread_t upload_thread = RT_NULL;
//读取心率传感器线程
static rt_thread_t hrate_thread = RT_NULL;
//计算心率线程
static rt_thread_t calc_thread = RT_NULL;

static void upload_entry(void *parameter)
{
	while (1)
	{
		//检查是否有可以用于上传的心率数据
		if (rt_sem_trytake(report_ready) == RT_EOK)
		{
			send_info(0x01, hrate_rst);
			rt_kprintf("send hrate %i\n\n", hrate_rst);
		}
		rt_thread_mdelay(100);
	}
}

static void hrate_entry(void *parameter)
{
	rt_uint16_t hrate_cnt = 0;
	buf_page = 0;

	while (1)
	{
		//检查传感器是否有新数据
		//此处信号量由userirt中的GPIO中断处理函数发送
		if (rt_sem_trytake(hrate_ready) == RT_EOK)
		{
			get_IRV(&buf[buf_page][0][hrate_cnt], &buf[buf_page][1][hrate_cnt]);
			hrate_cnt++;
			//记录5秒，换页，设置计算信号量，使得计算线程开始计算
			if (hrate_cnt == 500)
			{
				buf_page ^= 1;
				hrate_cnt = 0;
				rt_sem_release(calc_ready);
			}
		}
		rt_thread_mdelay(1);
	}
}

static void calc_entry(void *parameter)
{
	rt_int8_t spo2V, hrateV;
	rt_int32_t spo2, hrate;
	rt_int32_t dataV;
	while (1)
	{
		if (rt_sem_trytake(calc_ready) == RT_EOK)
		{
			rt_kprintf("calc\n");
			//检查数据是否有效（若手指不在传感器上，读数较小）
			dataV = 6;
			for (int i = 0; i < 500; i++)
			{
				if (buf[1 ^ buf_page][0][i] < 40000)
					dataV--;
				if (buf[1 ^ buf_page][1][i] < 40000)
					dataV--;
			}
			if (dataV < 0)
			{
				rt_kprintf("data invalid\n\n");
				continue;
			}
			//调用MAXIM提供的库进行计算
			maxim_heart_rate_and_oxygen_saturation(&buf[1 ^ buf_page][1][0], 500,
												   &buf[1 ^ buf_page][0][0], &spo2, &spo2V, &hrate, &hrateV);
			//由于血氧读数极不稳定，不使用
			//rt_kprintf("spo2 %i,%i;hrate %i,%i\n",spo2,spo2V,hrate,hrateV);
			rt_kprintf("hrate %i,%i\n", hrate, hrateV);
			if (hrateV != 0)
			{
				hrate_rst = hrate;
				rt_sem_release(report_ready);
			}
		}
		rt_thread_mdelay(1);
	}
}

int main(void)
{
	//信号量初始化
	hrate_ready = rt_sem_create("DRD", 0, RT_IPC_FLAG_FIFO);
	calc_ready = rt_sem_create("CAL", 0, RT_IPC_FLAG_FIFO);
	report_ready = rt_sem_create("RPT", 0, RT_IPC_FLAG_FIFO);
	
	//外部设备及中断初始化
	up_init();
	hrate_init();
	irt_init();

	//线程初始化
	upload_thread = rt_thread_create("upload",
									 upload_entry, RT_NULL, 512, 19, 5);
	hrate_thread = rt_thread_create("hrate",
									hrate_entry, RT_NULL, 512, 18, 5);
	calc_thread = rt_thread_create("calc",
								   calc_entry, RT_NULL, 8192, 20, 5);

	rt_thread_startup(upload_thread);
	rt_thread_startup(calc_thread);
	rt_thread_startup(hrate_thread);

	while (1)
		rt_thread_mdelay(1000);
}
