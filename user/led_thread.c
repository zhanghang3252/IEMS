#include "rt_thread_function.h"
#include "led.h"
/* 线程 1 的入口函数 */
void led_thread(void *parameter)
{
	led_init();
	while(RT_TRUE)
	{
		led_on();
		rt_thread_delay(1000);
		led_off();	
		rt_thread_delay(1000);
	}
}


