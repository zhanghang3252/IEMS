#include "main.h"

uint8_t rx485_buf[20];



uint8_t usart_rx_buf[20];

int main(void)
{	
	systick_config();
	led_init();
	usart0_init(115200);
	//usart2_init(115200);
	//can0_init();
	//rs485_init();
	
	printf("\r\n system start \r\n");
	if (sd_io_init() == SD_OK)//SDø®≥ı ºªØ
		printf("\r\n Card init success!\r\n");
	else
		printf("\r\n Card init failed!\r\n");
		
	while(1)
	{	
		usart_send_data(USART0,(uint8_t *)"serial test\r\n");
//		usart_receive_data(usart_rx_buf);
//		can0_send_test();				
//    rs485_send_data(1);
//		rs485_receive_data(rx485_buf);
//		printf("rx485_buf£∫%s",rx485_buf);
//		led_on();
//		delay_1ms(1000);
//		led_off();
		delay_1ms(1000);	
	}	
}






