#include "main.h"
#include "rt_thread_function.h"
#include <rtthread.h>

int main(void)
{	
	
//	usart2_init(115200);
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
	rt_thread_t led_thread_tid = rt_thread_create("led_thread",led_thread, RT_NULL,128,1,10);
	/* �������߳̿��ƿ飬��������߳� */
	if (led_thread_tid != RT_NULL)
		rt_thread_startup(led_thread_tid);
}





