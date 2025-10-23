#include "rt_thread_function.h"
#include "main.h"

#define USART_RX_BUF_SIZE 512  // ���ջ�������С����1�ֽڸ�'\0'��

uint16_t usart_rx_flag=0;
uint8_t usart_rx_buffer[USART_RX_BUF_SIZE];
uint16_t usart_rx_num=0;


/* �߳� wifi_thread ����ں��� */
void wifi_thread(void *parameter)
{
	usart2_init(115200);
	rt_thread_delay(1000);

	usart_send_data(USART2,"AT\r\n");

	while(RT_TRUE)
	{
		if(usart_rx_flag == 1)
		{
			usart_rx_flag=0;
			if(strcmp(usart_rx_buffer,"\r\nOK\r\n") == 0)
			{
				rt_kprintf("[%d]WIFI���Գɹ�\r\n",get_systick());
				memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // ��ջ������������������
			}
			else
			{
				rt_kprintf("WIFI����ʧ��\r\n");
				memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // ��ջ������������������
			}
		}
		rt_thread_delay(1000);
	}
}


// USART2�жϴ�����
void USART2_IRQHandler(void)
{
	rt_interrupt_enter();          //���ж���һ��Ҫ������Ժ����������ж�
    // 1. ����RBNE�жϣ��յ�1���ֽڣ�
    if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) && usart_flag_get(USART2, USART_FLAG_RBNE)) {
        // ��ֹ�����������Ԥ��1�ֽڸ�'\0'������������ USART_RX_BUF_SIZE-1 �ֽڣ�
        if(usart_rx_num < (USART_RX_BUF_SIZE - 1)){
            usart_rx_buffer[usart_rx_num++] = usart_data_receive(USART2);  // ����1�ֽ�
        }
				usart_flag_clear(USART2,USART_FLAG_RBNE);
        usart_interrupt_flag_clear(USART2, USART_INT_FLAG_RBNE);  // ���RBNE�жϱ�־
    }
    
    //����IDLE�жϣ�һ֡���ݽ�����ɣ�
    if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE) && usart_flag_get(USART2, USART_FLAG_IDLE)) {
        // ��ȷ���IDLE��־�������ȶ����ݼĴ����������־��
				// dummy read����ʵ����;�������־��
        uint32_t dummy = usart_data_receive(USART2); 
//        usart_rx_buffer[usart_rx_num] = '\n';
			
				rt_kprintf("%s",usart_rx_buffer);
			
        // �ȷ�����Ч���ݣ������ü��������ⷢ��ʱ��������0��
        usart_rx_flag = 1;    // ��ǽ�����ɣ�����������Ҫ�жϣ��ɱ�����
        usart_rx_num = 0;     // ���ý��ռ���
        //memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // ��ջ������������������
				usart_flag_clear(USART0,USART_FLAG_IDLE);
        usart_interrupt_flag_clear(USART2, USART_INT_FLAG_IDLE); // ���IDLE�жϱ�־
    }
		
		rt_interrupt_leave();    //���ж���һ��Ҫ������Ժ������뿪�ж�
}

