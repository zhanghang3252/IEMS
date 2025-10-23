#include "rt_thread_function.h"
#include "main.h"

#define USART_RX_BUF_SIZE 512  // 接收缓冲区大小（留1字节给'\0'）

uint16_t usart_rx_flag=0;
uint8_t usart_rx_buffer[USART_RX_BUF_SIZE];
uint16_t usart_rx_num=0;


/* 线程 wifi_thread 的入口函数 */
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
				rt_kprintf("[%d]WIFI测试成功\r\n",get_systick());
				memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // 清空缓冲区，避免残留数据
			}
			else
			{
				rt_kprintf("WIFI测试失败\r\n");
				memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // 清空缓冲区，避免残留数据
			}
		}
		rt_thread_delay(1000);
	}
}


// USART2中断处理函数
void USART2_IRQHandler(void)
{
	rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    // 1. 处理RBNE中断（收到1个字节）
    if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE) && usart_flag_get(USART2, USART_FLAG_RBNE)) {
        // 防止缓冲区溢出（预留1字节给'\0'，所以最大接收 USART_RX_BUF_SIZE-1 字节）
        if(usart_rx_num < (USART_RX_BUF_SIZE - 1)){
            usart_rx_buffer[usart_rx_num++] = usart_data_receive(USART2);  // 接收1字节
        }
				usart_flag_clear(USART2,USART_FLAG_RBNE);
        usart_interrupt_flag_clear(USART2, USART_INT_FLAG_RBNE);  // 清除RBNE中断标志
    }
    
    //处理IDLE中断（一帧数据接收完成）
    if(usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE) && usart_flag_get(USART2, USART_FLAG_IDLE)) {
        // 正确清除IDLE标志（必须先读数据寄存器，再清标志）
				// dummy read（无实际用途，仅清标志）
        uint32_t dummy = usart_data_receive(USART2); 
//        usart_rx_buffer[usart_rx_num] = '\n';
			
				rt_kprintf("%s",usart_rx_buffer);
			
        // 先发送有效数据，再重置计数（避免发送时计数被清0）
        usart_rx_flag = 1;    // 标记接收完成（若主函数需要判断，可保留）
        usart_rx_num = 0;     // 重置接收计数
        //memset(usart_rx_buffer, 0, USART_RX_BUF_SIZE);  // 清空缓冲区，避免残留数据
				usart_flag_clear(USART0,USART_FLAG_IDLE);
        usart_interrupt_flag_clear(USART2, USART_INT_FLAG_IDLE); // 清除IDLE中断标志
    }
		
		rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}

