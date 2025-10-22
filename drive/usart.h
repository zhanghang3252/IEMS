#ifndef __USART_H
#define __USART_H
#include "gd32f4xx.h"
#include "stdio.h"
#include "string.h"

#define USART_RX_BUF_SIZE 512  // 接收缓冲区大小（留1字节给'\0'）

void usart0_init(uint32_t baudval);
void usart2_init(uint32_t baudval);
void usart_send_data(uint32_t usart_periph, uint8_t* data);
void usart_send_data_num(uint32_t usart_periph, uint8_t* data,uint16_t data_num);
void usart_receive_data(uint8_t * usart_rx_buf);
extern uint8_t usart_rx_buffer[USART_RX_BUF_SIZE];
#endif


