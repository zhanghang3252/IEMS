#include "usart.h"

#define USART_RX_BUF_SIZE 128  // ���ջ�������С����1�ֽڸ�'\0'��
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
	usart_baudrate_set(USART0, baudval);//���ò�����
	usart_parity_config(USART0,USART_PM_NONE);//����У��λ ��У��
	usart_word_length_set(USART0,USART_WL_8BIT);//��������λ 8λ
	usart_stop_bit_set(USART0, USART_STB_1BIT);//����ֹͣλ 1λ
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
	usart_hardware_flow_coherence_config(USART0,USART_HCM_NONE);//��Ӳ������
	
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);//��������
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);//��������
	
//	usart_interrupt_enable(USART0, USART_INT_TC);//�����������
	usart_interrupt_enable(USART0, USART_INT_RBNE);//�����������ǿ�
	usart_interrupt_enable(USART0, USART_INT_IDLE);//��������������

	nvic_irq_enable(USART0_IRQn, 0, 0);
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
	usart_baudrate_set(USART2, baudval);//���ò�����
	usart_parity_config(USART2,USART_PM_NONE);//����У��λ ��У��
	usart_word_length_set(USART2,USART_WL_8BIT);//��������λ 8λ
	usart_stop_bit_set(USART2, USART_STB_1BIT);//����ֹͣλ 1λ
	usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
	usart_hardware_flow_coherence_config(USART2,USART_HCM_NONE);//��Ӳ������
	
	usart_receive_config(USART2, USART_RECEIVE_ENABLE);//��������
	usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);//��������
	
//	usart_interrupt_enable(USART2, USART_INT_TC);//�������
	usart_interrupt_enable(USART2, USART_INT_RBNE);//�������ǿ�
	usart_interrupt_enable(USART2, USART_INT_IDLE);//����������
	nvic_irq_enable(USART2_IRQn, 0, 0);
	
	usart_enable(USART2);
}



// �ض���printf������
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
    return ch;
}


//���ڷ���
void usart_send_data(uint32_t usart_periph, uint8_t* data)
{
	while(*data != '\0')
	{		
		usart_data_transmit(usart_periph,*data);
		while(RESET == usart_flag_get(usart_periph, USART_FLAG_TC));	
		data++;
	}
}



void usart_receive_data(uint8_t * usart_rx_buf)
{
	if(usart_rx_flag == 1)
	{
		usart_rx_flag=0;//���ձ�־λ�ÿ�
		usart_rx_num=0;//���ռ�������
		memcpy(usart_rx_buffer,usart_rx_buf,usart_rx_num);//��������
		printf("usart rx:%s\r\n",usart_rx_buf);//��ʾ��������
	}else{
		return;
	}
}


// USART0�жϴ�����
void USART0_IRQHandler(void)
{
    // 1. ����RBNE�жϣ��յ�1���ֽڣ�
    if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) && 
       usart_flag_get(USART0, USART_FLAG_RBNE)) 
    {
        // ��ֹ�����������Ԥ��1�ֽڸ�'\0'������������ USART_RX_BUF_SIZE-1 �ֽڣ�
        if(usart_rx_num < (USART_RX_BUF_SIZE - 1))
        {
            usart_rx_buffer[usart_rx_num++] = usart_data_receive(USART0);  // ����1�ֽ�
        }
				usart_flag_clear(USART0,USART_FLAG_RBNE);
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);  // ���RBNE�жϱ�־
    }
    
    // 2. ����IDLE�жϣ�һ֡���ݽ�����ɣ�
    if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE) && 
       usart_flag_get(USART0, USART_FLAG_IDLE)) 
    {
        // �ؼ�����1����ȷ���IDLE��־�������ȶ����ݼĴ����������־��
        uint32_t dummy = usart_data_receive(USART0);  // dummy read����ʵ����;�������־��
        (void)dummy;  // ����δʹ�ñ�������
        
        // �ؼ�����2������ַ���������'\0'��������ķ��ͺ���
        usart_rx_buffer[usart_rx_num] = '\0';  // ��Ч����ĩβ��'\0'��ȷ�����ͺ�������ֹͣ
        
        // �ؼ�����3���ȷ�����Ч���ݣ������ü��������ⷢ��ʱ��������0��
        usart_send_data(USART0, usart_rx_buffer);  // ���ͽ��յ��ַ���������Ч���ݣ�
        
        // ���ý���״̬��׼���´ν���
        usart_rx_flag = 1;    // ��ǽ�����ɣ�����������Ҫ�жϣ��ɱ�����
        usart_rx_num = 0;     // ���ý��ռ���
        memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // ��ѡ����ջ������������������
        
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);  // ���IDLE�жϱ�־
    }
}




