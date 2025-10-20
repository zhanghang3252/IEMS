#include "led.h"

void  led_init (void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN_13);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13);
	GPIO_BC(GPIOC) = GPIO_PIN_13;
	gpio_bit_set(GPIOC, GPIO_PIN_13);
}

void led_on(void)
{
	gpio_bit_reset(GPIOC, GPIO_PIN_13);
}

void led_off(void)
{
	gpio_bit_set(GPIOC, GPIO_PIN_13);
}


