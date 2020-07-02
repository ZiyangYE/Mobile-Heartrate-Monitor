/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       		Notes
 * 2020-03-30     NUAAQIANJIN       the first version
 */

#include "max30102.h"

rt_err_t maxim_max30102_write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t i2c_addr,
                                  rt_uint8_t i2c_data)
{
    struct rt_i2c_msg msgs;
    rt_uint8_t send_data[2];
    send_data[0] = i2c_addr;
    send_data[1] = i2c_data;

    msgs.addr = MAX30102_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = send_data;
    msgs.len = 2;
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return RT_ERROR;
    }
}

rt_err_t maxim_max30102_read_reg(struct rt_i2c_bus_device *bus, rt_uint8_t i2c_addr,
                                 rt_uint8_t *i2c_data)

{
    struct rt_i2c_msg msgs[2];
    rt_uint8_t send_data;
    send_data = i2c_addr;

    msgs[0].addr = MAX30102_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &send_data;
    msgs[0].len = 1;

    msgs[1].addr = MAX30102_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = i2c_data;
    msgs[1].len = 1;

    if (rt_i2c_transfer(bus, msgs, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return RT_ERROR;
    }
}

//读取信息，并转化为最终的数值
rt_err_t maxim_max30102_read_fifo(struct rt_i2c_bus_device *bus,
                                  rt_uint32_t *pun_red_led, rt_uint32_t *pun_ir_led)

{
    rt_uint32_t un_temp;
    rt_uint8_t uch_temp;
    rt_uint8_t ach_i2c_data[6];
    struct rt_i2c_msg msgs[2];

    *pun_red_led = 0;
    *pun_ir_led = 0;

    //read and clear status register
    maxim_max30102_read_reg(bus, REG_INTR_STATUS_1, &uch_temp);
    maxim_max30102_read_reg(bus, REG_INTR_STATUS_2, &uch_temp);

    uch_temp = REG_FIFO_DATA;
    msgs[0].addr = MAX30102_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &uch_temp;
    msgs[0].len = 1;

    msgs[1].addr = MAX30102_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = ach_i2c_data;
    msgs[1].len = 6;

    if (rt_i2c_transfer(bus, msgs, 2) != 2)
    {
        return RT_ERROR;
    }

    //rt_kprintf("%s\n", (char *)ach_i2c_data);
    un_temp = (rt_uint32_t)ach_i2c_data[0];
    un_temp <<= 16;
    *pun_red_led += un_temp;
    un_temp = (rt_uint32_t)ach_i2c_data[1];
    un_temp <<= 8;
    *pun_red_led += un_temp;
    un_temp = (rt_uint32_t)ach_i2c_data[2];
    *pun_red_led += un_temp;

    un_temp = (rt_uint32_t)ach_i2c_data[3];
    un_temp <<= 16;
    *pun_ir_led += un_temp;
    un_temp = (rt_uint32_t)ach_i2c_data[4];
    un_temp <<= 8;
    *pun_ir_led += un_temp;
    un_temp = (rt_uint32_t)ach_i2c_data[5];
    *pun_ir_led += un_temp;
    *pun_red_led &= 0x03FFFF; //Mask MSB [23:18]
    *pun_ir_led &= 0x03FFFF;  //Mask MSB [23:18]

    return RT_EOK;
}

rt_err_t maxim_max30102_reset(struct rt_i2c_bus_device *bus)
{
    if (maxim_max30102_write_reg(bus, REG_MODE_CONFIG, 0x40) == 1)
        return RT_EOK;
    else
        return RT_ERROR;
}

rt_err_t maxim_max30102_init(struct rt_i2c_bus_device *bus)
{
    rt_uint8_t temp;
    //	rt_thread_mdelay(2);

    // INTR setting
    if (maxim_max30102_write_reg(bus, REG_INTR_ENABLE_1, 0xc0) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_INTR_ENABLE_1, &temp);
    rt_kprintf("REG_INTR_ENABLE_1: %x\n", temp);

    //	rt_thread_mdelay(2);
    if (maxim_max30102_write_reg(bus, REG_INTR_ENABLE_2, 0x00) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_INTR_ENABLE_2, &temp);
    rt_kprintf("REG_INTR_ENABLE_2: %x\n", temp);

    //	rt_thread_mdelay(2);
    //FIFO_WR_PTR[4:0]
    if (maxim_max30102_write_reg(bus, REG_FIFO_WR_PTR, 0x00) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_FIFO_WR_PTR, &temp);
    rt_kprintf("REG_FIFO_WR_PTR: %x\n", temp);

    //	rt_thread_mdelay(2);
    //OVF_COUNTER[4:0]
    if (maxim_max30102_write_reg(bus, REG_OVF_COUNTER, 0x00) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_OVF_COUNTER, &temp);
    rt_kprintf("REG_OVF_COUNTER: %x\n", temp);

    //	rt_thread_mdelay(2);
    //FIFO_RD_PTR[4:0
    if (maxim_max30102_write_reg(bus, REG_FIFO_RD_PTR, 0x00) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_FIFO_RD_PTR, &temp);
    rt_kprintf("REG_FIFO_RD_PTR: %x\n", temp);

    //	rt_thread_mdelay(2);
    //sample avg = 1, fifo rollover=false, fifo almost full = 17
    if (maxim_max30102_write_reg(bus, REG_FIFO_CONFIG, 0x0f) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_FIFO_CONFIG, &temp);
    rt_kprintf("REG_FIFO_CONFIG: %x\n", temp);

    //	rt_thread_mdelay(2);
    //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
    if (maxim_max30102_write_reg(bus, REG_MODE_CONFIG, 0x03) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_MODE_CONFIG, &temp);
    rt_kprintf("REG_MODE_CONFIG: %x\n", temp);

    //	rt_thread_mdelay(2);
    // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
    if (maxim_max30102_write_reg(bus, REG_SPO2_CONFIG, 0x27) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_SPO2_CONFIG, &temp);
    rt_kprintf("REG_SPO2_CONFIG: %x\n", temp);

    //	rt_thread_mdelay(2);
    //Choose value for ~ 7mA for LED1
    if (maxim_max30102_write_reg(bus, REG_LED1_PA, 0x24) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_LED1_PA, &temp);
    rt_kprintf("REG_LED1_PA: %x\n", temp);

    //	rt_thread_mdelay(2);
    // Choose value for ~ 7mA for LED2
    if (maxim_max30102_write_reg(bus, REG_LED2_PA, 0x24) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_LED2_PA, &temp);
    rt_kprintf("REG_LED2_PA: %x\n", temp);

    //	rt_thread_mdelay(2);
    // Choose value for ~ 25mA for Pilot LED
    if (maxim_max30102_write_reg(bus, REG_PILOT_PA, 0x7f) == RT_ERROR)
        return RT_ERROR;
    maxim_max30102_read_reg(bus, REG_PILOT_PA, &temp);
    rt_kprintf("REG_PILOT_PA: %x\n", temp);

    //	rt_thread_mdelay(2);
    maxim_max30102_read_reg(bus, 0, &temp);
    rt_kprintf("%x\n", temp);
    return RT_EOK;
}
