#include "usart.h"

void usart0_init(uint32_t baudval)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_USART0);

	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_15);
	gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_3);
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);	

	usart_deinit(USART0);
	usart_baudrate_set(USART0, baudval);//设置波特率
	usart_parity_config(USART0,USART_PM_NONE);//设置校验位 无校验
	usart_word_length_set(USART0,USART_WL_8BIT);//设置数据位 8位
	usart_stop_bit_set(USART0, USART_STB_1BIT);//设置停止位 1位
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
	usart_hardware_flow_coherence_config(USART0,USART_HCM_NONE);//无硬件流控
	
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);//开启接收
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);//开启发送
	
//	usart_interrupt_enable(USART0, USART_INT_TC);//开启发送完成
	usart_interrupt_enable(USART0, USART_INT_RBNE);//开启接收区非空
//	usart_interrupt_enable(USART0, USART_INT_IDLE);//开启接收区空闲

	nvic_irq_enable(USART0_IRQn,1,0);
	usart_enable(USART0);
}


void usart2_init(uint32_t baudval)
{
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_USART2);

	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_8);
	gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_9);
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);	

	usart_deinit(USART2);
	usart_baudrate_set(USART2, baudval);//设置波特率
	usart_parity_config(USART2,USART_PM_NONE);//设置校验位 无校验
	usart_word_length_set(USART2,USART_WL_8BIT);//设置数据位 8位
	usart_stop_bit_set(USART2, USART_STB_1BIT);//设置停止位 1位
	usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
	usart_hardware_flow_coherence_config(USART2,USART_HCM_NONE);//无硬件流控
	
	usart_receive_config(USART2, USART_RECEIVE_ENABLE);//开启接收
	usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);//开启发送
	
//	usart_interrupt_enable(USART2, USART_INT_TC);//发送完成
	usart_interrupt_enable(USART2, USART_INT_RBNE);//接收区非空
	usart_interrupt_enable(USART2, USART_INT_IDLE);//接收区空闲
	nvic_irq_enable(USART2_IRQn, 0, 0);
	
	usart_enable(USART2);
}



//// 重定向printf到串口
//int fputc(int ch, FILE *f)
//{
//    usart_data_transmit(USART0, (uint8_t)ch);
//    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
//    return ch;
//}


//串口发送
void usart_send_data(uint32_t usart_periph, uint8_t* data)
{
	while(*data != '\0')
	{		
		usart_data_transmit(usart_periph,*data);
		while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC));	
		data++;
	}
}


//串口发送 带已知大小
void usart_send_data_num(uint32_t usart_periph, uint8_t* data,uint16_t data_num)
{
	for(uint16_t temp=0;temp<data_num;temp++)
	{
		usart_data_transmit(usart_periph,data[temp]);
		while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC));	
	}
}


