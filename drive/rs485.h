#ifndef __RS485_H
#define __RS485_H
#include "main.h"
void rs485_init(void);
void usart1_init(void);
// RS485 ���ͺ���
void rs485_send_data(uint8_t data);
// RS485 ���պ���
void rs485_receive_data(uint8_t *data);
#endif


