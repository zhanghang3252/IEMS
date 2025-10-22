#include "main.h"

uint8_t rx485_buf[20];

uint8_t usart_rx_buf[20];

int main(void)
{	
	systick_config();
	led_init();
	usart0_init(115200);
//	usart2_init(115200);
	i2c_init();
	
	i2c_eeprom_init();
	
	//can0_init();
	//rs485_init();
	
	printf("system start \r\n");
	if (sd_io_init() == SD_OK)//SDø®≥ı ºªØ
		printf("Card init success!\r\n");
	else
		printf("Card init failed!\r\n");
	i2c_24c02_test();
	while(1)
	{	
//		usart_send_data_num(USART2,"AT\r\n",sizeof("AT\r\n"));
//		can0_send_test();				
//    rs485_send_data(1);
//		rs485_receive_data(rx485_buf);
//		printf("rx485_buf£∫%s",rx485_buf);
		led_on();
		delay_1ms(1000);
		led_off();
		delay_1ms(1000);	
	}	
}






