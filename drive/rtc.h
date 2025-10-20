#ifndef __RTC_H
#define __RTC_H
#include "gd32f4xx.h"
#include "stdio.h"

typedef struct time_t{
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}time_t;
uint8_t rtc_setup(time_t time);
void rtc_get_time(time_t *time);
#endif


