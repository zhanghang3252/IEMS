#ifndef __USART_H
#define __USART_H
#include "gd32f4xx.h"
#include "stdio.h"
#include "string.h"

void usart0_init(uint32_t baudval);
void usart2_init(uint32_t baudval);
void usart_send_data(uint32_t usart_periph, uint8_t* data);
void usart_send_data_num(uint32_t usart_periph, uint8_t* data,uint16_t data_num);
#endif


