#include "main.h"
#include "rt_thread_function.h"
#include <rtthread.h>

int main(void)
{	
//	i2c_init();
//	i2c_eeprom_init();
	
	//can0_init();
	//rs485_init();
	
//	rt_kprintf("system start \r\n");
//	if (sd_io_init() == SD_OK)//SD����ʼ��
//		rt_kprintf("Card init success!\r\n");
//	else
//		rt_kprintf("Card init failed!\r\n");
	
	/* �����߳� 1�������� thread1������� thread1_entry*/
	static int led_delay =100;
	rt_thread_t led_thread_tid = rt_thread_create("led_thread",led_thread,&led_delay,128,1,10);
	if (led_thread_tid != RT_NULL){
		rt_thread_startup(led_thread_tid);/* �������߳̿��ƿ飬��������߳� */
	}	
	
	rt_thread_t wifi_thread_tid = rt_thread_create("wifi_thread",wifi_thread,RT_NULL,512,0,10);
	if (wifi_thread_tid != RT_NULL){
		rt_thread_startup(wifi_thread_tid);/* �������߳̿��ƿ飬��������߳� */
	}
	
	while(1){
	
		rt_thread_delay(10);
	}
}





