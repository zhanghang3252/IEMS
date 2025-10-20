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
	//usart���ų�ʼ��
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_2);//���Ÿ�������
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_3);//���Ÿ�������
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);//����ģʽ����
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);	
	
	//RS485 DE���ų�ʼ��
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_1);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
}

void rs485_usart_init(void)
{
	usart_deinit(USART1);
	usart_baudrate_set(USART1, 9600);//���ò�����
	usart_parity_config(USART1,USART_PM_NONE);//����У��λ ��У��
	usart_word_length_set(USART1,USART_WL_8BIT);//��������λ 8λ
	usart_stop_bit_set(USART1, USART_STB_1BIT);//����ֹͣλ 1λ
	usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
	usart_hardware_flow_coherence_config(USART1,USART_HCM_NONE);//��Ӳ������
	
	usart_receive_config(USART1, USART_RECEIVE_ENABLE);//��������
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);//��������
	
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);
//	usart_interrupt_enable(USART1, USART_INT_TC);//��������ж�
	usart_interrupt_enable(USART1, USART_INT_RBNE);//�����ж�
	usart_interrupt_enable(USART1, USART_INT_IDLE);//��������ж�
	nvic_irq_enable(USART1_IRQn, 0, 1);
	
	usart_enable(USART1);
}


void rs485_init(void)
{

	rs485_clock_init();//ʱ�ӳ�ʼ����
	rs485_pin_init();//���ų�ʼ��
	rs485_usart_init();//���ڳ�ʼ��
	
	//Ĭ������Ϊ����ʹ��
	gpio_bit_write(GPIOA,GPIO_PIN_1,RESET);
}

// RS485 ���ͺ���
void rs485_send_data(uint8_t data)
{
		//����Ϊ����ʹ��
	gpio_bit_write(GPIOA,GPIO_PIN_1,SET);
	
	while(RESET == usart_flag_get(USART2, USART_FLAG_TBE));	// �ȴ�������Ϊ��
	
	usart_data_transmit(USART1,(uint8_t)data);
	
  while(RESET == usart_flag_get(USART1, USART_FLAG_TC));// �ȴ��������
		//����Ϊ����ʹ��
	gpio_bit_write(GPIOA,GPIO_PIN_1,RESET);
}


// RS485 ���պ���
uint8_t receive_ok = 0;
void rs485_receive_data(uint8_t *data)
{
	while(receive_ok != 1);
	memcpy(data,rs485_rx_buf,20);
	receive_ok=0;
}



//USART1�жϴ���
void USART1_IRQHandler(void)
{
//	// ���������
//	if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_TC)) 
//	{ 
//		
//		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_TC);
//		usart_flag_clear(USART1, USART_FLAG_TC); // �ֶ���� TC ��־
//	}

	//��������ж�
	if((RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)) &&
					(RESET != usart_flag_get(USART1, USART_FLAG_RBNE))) 
	{			
		rs485_rx_buf[rx_num++] = usart_data_receive(USART1);
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE);
		usart_flag_clear(USART1,USART_FLAG_RBNE);							
	}
	else if (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)) //������� 
	{
		rx_num=0;
		receive_ok=1;
		usart_interrupt_flag_clear(USART1, USART_INT_FLAG_IDLE);
		usart_data_receive(USART1); /* ���������ɱ�־λ */
	}
}
						


