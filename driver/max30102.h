/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-30     NUAAQIANJIN       the first version
 */
#ifndef APPLICATIONS_MAX30102_H_
#define APPLICATIONS_MAX30102_H_

#include <rtthread.h>
#include <rtdevice.h>
/*
#define I2C_WRITE_ADDR 	0xAE//	0B 1010 1110 >>1 0B 0101 0111
#define I2C_READ_ADDR 	0xAF// 	0B 1010 1111
*/
#define MAX30102_ADDR 0x57

//register addresses
#define REG_INTR_STATUS_1 0x00
#define REG_INTR_STATUS_2 0x01
#define REG_INTR_ENABLE_1 0x02
#define REG_INTR_ENABLE_2 0x03
#define REG_FIFO_WR_PTR 0x04
#define REG_OVF_COUNTER 0x05
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA 0x07
#define REG_FIFO_CONFIG 0x08
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D
#define REG_PILOT_PA 0x10
#define REG_MULTI_LED_CTRL1 0x11
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR 0x1F
#define REG_TEMP_FRAC 0x20
#define REG_TEMP_CONFIG 0x21
#define REG_PROX_INT_THRESH 0x30
#define REG_REV_ID 0xFE
#define REG_PART_ID 0xFF

rt_err_t maxim_max30102_init(struct rt_i2c_bus_device *bus);
rt_err_t maxim_max30102_read_reg(struct rt_i2c_bus_device *bus, rt_uint8_t i2c_addr,
                                 rt_uint8_t *i2c_data);
rt_err_t maxim_max30102_write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t i2c_addr,
                                  rt_uint8_t i2c_data);
rt_err_t maxim_max30102_read_fifo(struct rt_i2c_bus_device *bus,
                                  rt_uint32_t *pun_red_led, rt_uint32_t *pun_ir_led);
rt_err_t maxim_max30102_reset(struct rt_i2c_bus_device *bus);

//bool maxim_max30102_init();
//bool maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led);
//bool maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data);
//bool maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data);
//bool maxim_max30102_reset(void);

#endif /* APPLICATIONS_MAX30102_H_ */
