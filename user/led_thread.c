#include "rt_thread_function.h"
#include "led.h"
/* �߳� led_thread ����ں��� */
void led_thread(void *parameter)
{
	led_init();
	while(RT_TRUE)
	{
		led_on();
		rt_thread_delay(*(int*)parameter);
		led_off();	
		rt_thread_delay(*(int*)parameter);
	}
}


