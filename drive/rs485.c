#include "rs485.h"
uint8_t rs485_rx_buf[20];
int rx_num=0;

void rs485_clock_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART1);
}

void rs485_pin_init(void)
{
	//usart引脚初始化
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_2);//引脚复用配置
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_3);//引脚复用配置
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);//引脚模式配置
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);	
	
	//RS485 DE引脚初始化
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_1);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}

void rs485_usart_init(void)
{
	usart_deinit(USART1);
	usart_baudrate_set(USART1, 9600);//设置波特率
	usart_parity_config(USART1,USART_PM_NONE);//设置校验位 无校验
	usart_word_length_set(USART1,USART_WL_8BIT);//设置数据位 8位
	usart_stop_bit_set(USART1, USART_STB_1BIT);//设置停止位 1位
	usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
	usart_hardware_flow_coherence_config(USART1,USART_HCM_NONE);//无硬件流控
	
	usart_receive_config(USART1, USART_RECEIVE_ENABLE);//开启接收
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);//开启发送
	
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);
//	usart_interrupt_enable(USART1, USART_INT_TC);//发送完成中断
	usart_interrupt_enable(USART1, USART_INT_RBNE);//接收中断
	usart_interrupt_enable(USART1, USART_INT_IDLE);//接收完成中断
	nvic_irq_enable(USART1_IRQn, 0, 1);
	
	usart_enable(USART1);
}


void rs485_init(void)
{

	rs485_clock_init();//时钟初始化化
	rs485_pin_init();//引脚初始化
	rs485_usart_init();//串口初始化
	
	//默认配置为接收使能
	gpio_bit_write(GPIOA,GPIO_PIN_1,RESET);
}

// RS485 发送函数
void rs485_send_data(uint8_t data)
{
		//配置为发送使能
	gpio_bit_write(GPIOA,GPIO_PIN_1,SET);
	
	while(RESET == usart_flag_get(USART2, USART_FLAG_TBE));	// 等待发送区为空
	
	usart_data_transmit(USART1,(uint8_t)data);
	
  while(RESET == usart_flag_get(USART1, USART_FLAG_TC));// 等待发送完成
		//配置为接收使能
	gpio_bit_write(GPIOA,GPIO_PIN_1,RESET);
}


// RS485 接收函数
uint8_t receive_ok = 0;
void rs485_receive_data(uint8_t *data)
{
	while(receive_ok != 1);
	memcpy(data,rs485_rx_buf,20);
	receive_ok=0;
}



//USART1中断处理
void USART1_IRQHandler(void)
{
//	// 处理发送完成
//	if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_TC)) 
//	{ 
//		
//		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_TC);
//		usart_flag_clear(USART1, USART_FLAG_TC); // 手动清除 TC 标志
//	}

	//处理接收中断
	if((RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)) &&
					(RESET != usart_flag_get(USART1, USART_FLAG_RBNE))) 
	{			
		rs485_rx_buf[rx_num++] = usart_data_receive(USART1);
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE);
		usart_flag_clear(USART1,USART_FLAG_RBNE);							
	}
	else if (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)) //接收完成 
	{
		rx_num=0;
		receive_ok=1;
		usart_interrupt_flag_clear(USART1, USART_INT_FLAG_IDLE);
		usart_data_receive(USART1); /* 清除接收完成标志位 */
	}
}
						


