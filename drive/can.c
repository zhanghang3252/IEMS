#include "can.h"


void can0_clock_init(void)
{
	/* enable CAN clock */
	rcu_periph_clock_enable(RCU_CAN0);
	rcu_periph_clock_enable(RCU_GPIOD);
}

void can0_pin_init(void)
{
	/* configure CAN0 GPIO */
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
	gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_0);

	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
	gpio_af_set(GPIOD, GPIO_AF_9, GPIO_PIN_1);
}

void can_nviv_init(void)
{
	nvic_irq_enable(CAN0_RX0_IRQn,0,0);
	/*启用可以接收fifo不空中断*/
	can_interrupt_enable(CAN0, CAN_INT_RFNE0);
}

void can0_config(void )
{
	can_parameter_struct            can_parameter;
	can_filter_parameter_struct     can_filter;

	can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
	can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);

	can_deinit(CAN0);
	can_parameter.time_triggered = DISABLE;
	can_parameter.auto_bus_off_recovery = ENABLE;
	can_parameter.auto_wake_up = DISABLE;
	can_parameter.auto_retrans = ENABLE;
	can_parameter.rec_fifo_overwrite = DISABLE;
	can_parameter.trans_fifo_order = DISABLE;
	can_parameter.working_mode = CAN_NORMAL_MODE;
	can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
	can_parameter.time_segment_1 = CAN_BT_BS1_8TQ;
	can_parameter.time_segment_2 = CAN_BT_BS2_1TQ;
	can_parameter.prescaler = 20;//250kbit/s

	/* initialize CAN */
	can_init(CAN0, &can_parameter);

	/* initialize filter */ 
	can_filter.filter_number=0;
	can_filter.filter_mode = CAN_FILTERMODE_MASK;
	can_filter.filter_bits = CAN_FILTERBITS_32BIT;
	can_filter.filter_list_high = 0x0000;
	can_filter.filter_list_low = 0x0000;
	can_filter.filter_mask_high = 0x0000;
	can_filter.filter_mask_low = 0x0000;
	can_filter.filter_fifo_number = CAN_FIFO0;
	can_filter.filter_enable = ENABLE;

	can_filter_init(&can_filter);
}


void can0_init(void)
{
	can0_clock_init();
	can0_pin_init();
	can_nviv_init();
	can0_config();
}

can_trasnmit_message_struct transmit_message;
uint8_t transmit_mailbox = 0;
can_receive_message_struct receive_message;
FlagStatus can0_receive_flag;
FlagStatus can0_error_flag = RESET;

void can0_send_test(void)
{

	transmit_message.tx_sfid = 0x7ab;
	transmit_message.tx_efid = 0x00;
	transmit_message.tx_ft = CAN_FT_DATA;
	transmit_message.tx_ff = CAN_FF_STANDARD;
	transmit_message.tx_dlen = 8;
	transmit_message.tx_data[0] = 0xA0;
	transmit_message.tx_data[1] = 0xA1;
	transmit_message.tx_data[2] = 0xA2;
	transmit_message.tx_data[3] = 0xA3;
	transmit_message.tx_data[4] = 0xA4;
	transmit_message.tx_data[5] = 0xA5;
	transmit_message.tx_data[6] = 0xA6;
	transmit_message.tx_data[7] = 0xA7;
	printf("\r\n can0 transmit data:");
	for(int i = 0; i < transmit_message.tx_dlen; i++)
	{
		printf(" 0x%02x ", transmit_message.tx_data[i]);
	}
	transmit_mailbox = can_message_transmit(CAN0, &transmit_message);
	while((CAN_TRANSMIT_OK != can_transmit_states(CAN0, transmit_mailbox)))
	if(SET == can0_receive_flag)
	{
		can0_receive_flag = RESET;
		printf("can0 receive data:");
		for(int i = 0; i < receive_message.rx_dlen; i++){
				printf("%02x \r\n", receive_message.rx_data[i]);
		}
	}
}


/*!
    \brief      this function handles CAN0 RX0 exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void CAN0_RX0_IRQHandler(void)
{

    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
    
    if((0x32 == receive_message.rx_sfid)&&(CAN_FF_STANDARD == receive_message.rx_ff)&&(8 == receive_message.rx_dlen)){
        can0_receive_flag = SET; 
    }else{
        can0_error_flag = SET; 
    }
}


