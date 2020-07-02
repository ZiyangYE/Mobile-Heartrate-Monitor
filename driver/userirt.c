#include "userirt.h"

#define INT_PIN GET_PIN(C, 6)

extern rt_sem_t hrate_ready;

rt_err_t irt_init(void)
{
	rt_pin_mode(INT_PIN, PIN_MODE_INPUT_PULLUP);
	rt_pin_attach_irq(INT_PIN, PIN_IRQ_MODE_FALLING, irt_handler, RT_NULL);
	rt_pin_irq_enable(INT_PIN, PIN_IRQ_ENABLE);

	return RT_EOK;
}
void irt_handler(void *args)
{
	rt_interrupt_enter();
	rt_sem_release(hrate_ready);
	rt_interrupt_leave();
}
