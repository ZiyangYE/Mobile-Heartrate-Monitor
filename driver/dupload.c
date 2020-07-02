#include "dupload.h"

rt_err_t up_init()
{
	at_response_t resp = RT_NULL;
	at_response_t respIP = RT_NULL;

	at_client_init("uart2", 512);

	resp = at_create_resp(512, 0, rt_tick_from_millisecond(1000));
	respIP = at_create_resp(512, 1, rt_tick_from_millisecond(1000));
	if (!(resp && respIP))
	{
		rt_kprintf("RESP ALLOC FAIL");
		return -RT_ERROR;
	}

	//由于AIR720复位很慢，故不在初始化中复位，默认其已上电启动完成
	rt_thread_mdelay(200);
	//尝试Ping百度，若Ping不通，进行上电设置
	if (at_exec_cmd(resp, "AT+CIPPING=\"baidu.com\",1") != RT_EOK)
	{
		if (at_exec_cmd(resp, "AT+CSTT") != RT_EOK)
		{
			rt_kprintf("CSTT FAIL\n");
			return -RT_ERROR;
		}
		rt_thread_mdelay(200);

		if (at_exec_cmd(resp, "AT+CIICR") != RT_EOK)
		{
			rt_kprintf("CIICR FAIL\n");
			return -RT_ERROR;
		}
		rt_thread_mdelay(200);

		if (at_exec_cmd(respIP, "AT+CIFSR") != RT_EOK)
		{
			rt_kprintf("CIFSR FAIL\n");
			return -RT_ERROR;
		}
		rt_thread_mdelay(200);

		if (at_exec_cmd(resp, "AT+CIPPING=\"baidu.com\",1") != RT_EOK)
		{
			rt_kprintf("PING FAIL\n");
			return -RT_ERROR;
		}
		rt_thread_mdelay(200);

		if (at_exec_cmd(resp, "AT+CIPSTART=\"UDP\",\"IP-ADDRESS\",PORT") != RT_EOK)
		{
			rt_kprintf("BIND FAIL\n");
			return -RT_ERROR;
		}
		rt_thread_mdelay(200);
	}
	rt_kprintf("720H INIT FIN\n");

	at_delete_resp(resp);
	at_delete_resp(respIP);

	return RT_EOK;
}

rt_err_t send_info(uint8_t devNum, uint8_t heartRate)
{
	static uint8_t sendBuf[4];
	static at_response_t resp = RT_NULL;
	sendBuf[0] = 0;
	sendBuf[1] = devNum;
	sendBuf[2] = heartRate;
	sendBuf[3] = 0 ^ devNum ^ heartRate;
	if (resp == RT_NULL)
	{
		resp = at_create_resp(512, 1, rt_tick_from_millisecond(1000));
		if (!resp)
		{
			rt_kprintf("RESP ALLOC FAIL");
			return -RT_ERROR;
		}
	}
	if (at_exec_cmd(resp, "AT+CIPSEND=4") != RT_EOK)
	{
		rt_kprintf("START SEND FAIL\n");
		return -RT_ERROR;
	}
	rt_thread_mdelay(100);

	if (at_client_send((const char *)&sendBuf, 4) != 4)
	{
		rt_kprintf("SEND FAIL\n");
		return -RT_ERROR;
	}
	rt_thread_mdelay(100);

	return RT_EOK;
}
