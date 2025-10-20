#include "rtc.h"

rtc_parameter_struct rtc_initpara;

uint32_t RTCSRC_FLAG = 0;


uint8_t rtc_setup(time_t time)
{
	__IO uint32_t prescaler_a = 0, prescaler_s = 0;
  rcu_periph_clock_enable(RCU_PMU);
  pmu_backup_write_enable();
	
	rcu_osci_on(RCU_LXTAL);
	rcu_osci_stab_wait(RCU_LXTAL);
	rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

 prescaler_s = 0xFF;
 prescaler_a = 0x7F;
 rcu_periph_clock_enable(RCU_RTC);
 rtc_register_sync_wait();
	
	
	RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);
	
	rtc_initpara.factor_asyn = prescaler_a;
	rtc_initpara.factor_syn = prescaler_s;
	rtc_initpara.year = time.year;
	rtc_initpara.day_of_week = RTC_SATURDAY;
	rtc_initpara.month = time.month;
	rtc_initpara.date = time.date;
	rtc_initpara.display_format = RTC_24HOUR;
	rtc_initpara.am_pm = RTC_AM;

	rtc_initpara.hour = time.hour;
	rtc_initpara.minute = time.minute;
	rtc_initpara.second = time.second;

	/* RTC current time configuration */
	if(ERROR == rtc_init(&rtc_initpara)){
		printf("\n\r** RTC time configuration failed! **\n\r");
		return 0;
	}
	return 1;
}

void rtc_get_time(time_t *time)
{

	rtc_current_time_get(&rtc_initpara);
	time->year = rtc_initpara.year;
	time->month = rtc_initpara.month;
	time->date = rtc_initpara.date;
	time->hour = rtc_initpara.hour;
	time->minute = rtc_initpara.minute;
	time->second = rtc_initpara.second;
//    /* get the subsecond value of current time, and convert it into fractional format */
//    time_subsecond = rtc_subsecond_get();
//    subsecond_ss=(1000-(time_subsecond*1000+1000)/400)/100;
//    subsecond_ts=(1000-(time_subsecond*1000+1000)/400)%100/10;
//    subsecond_hs=(1000-(time_subsecond*1000+1000)/400)%10;

//    printf("Current time: %0.2x:%0.2x:%0.2x .%d%d%d \n\r", \
//          rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second,\
//          subsecond_ss, subsecond_ts, subsecond_hs);
}



