#include "hrate.h"

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;

rt_err_t hrate_init()
{
    rt_uint8_t result;
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find("i2c1");
    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("I2C MISS\n");
        return -RT_ERROR;
    }

    result = maxim_max30102_init(i2c_bus);
    if (result != RT_EOK)
    {
        rt_kprintf("30102 INIT FAIL\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}

rt_err_t get_IRV(rt_uint32_t *pun_red_led, rt_uint32_t *pun_ir_led)
{
    maxim_max30102_read_fifo(i2c_bus, pun_red_led, pun_ir_led);
    //rt_kprintf("ir = %i\n", *pun_ir_led);
    return RT_EOK;
}
