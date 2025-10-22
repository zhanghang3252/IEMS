#include "usart.h"

uint16_t usart_rx_flag=0;
uint8_t usart_rx_buffer[USART_RX_BUF_SIZE];
uint16_t usart_rx_num=0;

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

	nvic_irq_enable(USART0_IRQn,0,0);
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



// 重定向printf到串口
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
    return ch;
}


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


//串口发送 带大小
void usart_send_data_num(uint32_t usart_periph, uint8_t* data,uint16_t data_num)
{
	for(uint16_t temp=0;temp<data_num;temp++)
	{
		usart_data_transmit(usart_periph,data[temp]);
		while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC));	
	}
}


// USART2中断处理函数
void USART2_IRQHandler(void)
{
    // 1. 处理RBNE中断（收到1个字节）
    if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) && 
       usart_flag_get(USART2, USART_FLAG_RBNE)) 
    {
        // 防止缓冲区溢出（预留1字节给'\0'，所以最大接收 USART_RX_BUF_SIZE-1 字节）
        if(usart_rx_num < (USART_RX_BUF_SIZE - 1))
        {
            usart_rx_buffer[usart_rx_num++] = usart_data_receive(USART2);  // 接收1字节
        }
				usart_flag_clear(USART2,USART_FLAG_RBNE);
        usart_interrupt_flag_clear(USART2, USART_INT_FLAG_RBNE);  // 清除RBNE中断标志
    }
    
    // 2. 处理IDLE中断（一帧数据接收完成）
    if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE) && usart_flag_get(USART2, USART_FLAG_IDLE)) 
    {
        // 正确清除IDLE标志（必须先读数据寄存器，再清标志）
        uint32_t dummy = usart_data_receive(USART2); // dummy read（无实际用途，仅清标志）
        // 添加字符串结束符'\0'，适配发送函数
        usart_rx_buffer[usart_rx_num] = '\0';  // 有效数据末尾加'\0'，确保发送函数正常停止
				usart_send_data_num(USART0,usart_rx_buffer,usart_rx_num);//发送
        // 先发送有效数据，再重置计数（避免发送时计数被清0）
        // 重置接收状态，准备下次接收
        usart_rx_flag = 1;    // 标记接收完成（若主函数需要判断，可保留）
        usart_rx_num = 0;     // 重置接收计数
        memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // 可选：清空缓冲区，避免残留数据
				usart_flag_clear(USART0,USART_FLAG_IDLE);
        usart_interrupt_flag_clear(USART2, USART_INT_FLAG_IDLE);  // 清除IDLE中断标志
    }
}


