#include "gd32f4xx.h"
#include "i2c.h"
#include <stdio.h>
#include "systick.h"
#include "at24cxx.h"

uint8_t i2c_read_process = 0;
uint8_t i2c_write_process = 0;

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIO_I2C);

    /* connect I2C_SCL_PIN to I2C_SCL */
    gpio_af_set(I2C_SCL_PORT, I2C_GPIO_AF, I2C_SCL_PIN);
    /* connect I2C_SDA_PIN to I2C_SDA */
    gpio_af_set(I2C_SDA_PORT, I2C_GPIO_AF, I2C_SDA_PIN);
    /* configure GPIO pins of I2C */
    gpio_mode_set(I2C_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_PIN);
    gpio_output_options_set(I2C_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_mode_set(I2C_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_PIN);
    gpio_output_options_set(I2C_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
}

void i2c_nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(I2C0_EV_IRQn, 0, 2);
    nvic_irq_enable(I2C0_ER_IRQn, 0, 1);
}


/*!
    \brief      configure the I2CX interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_init(void)
{
		gpio_config();
		i2c_nvic_config();
    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C);
    /* configure I2C clock */
    i2c_clock_config(I2CX, I2C_SPEED, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2CX, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2CX_SLAVE_ADDRESS7);
    /* enable I2CX */
    i2c_enable(I2CX);
    /* enable acknowledge */
    i2c_ack_config(I2CX, I2C_ACK_ENABLE);
}

/*!
    \brief      handle I2CX event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2cx_event_irq_handler(void)
{
    if(RESET == i2c_process_flag) {
        switch(i2c_write_process) {
        case I2C_SEND_ADDRESS_FIRST:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_SBSEND)) {
                /* send slave address */
                i2c_master_addressing(I2CX, I2CX_SLAVE_ADDRESS7, I2C_TRANSMITTER);
                i2c_write_process = I2C_CLEAR_ADDRESS_FLAG_FIRST;
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG_FIRST:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_ADDSEND)) {
                /*clear ADDSEND bit */
                i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_ADDSEND);
                i2c_write_process = I2C_TRANSMIT_WRITE_READ_ADD;
            }
            break;
        case I2C_TRANSMIT_WRITE_READ_ADD:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_TBE)) {
                i2c_data_transmit(I2CX, i2c_write_dress);
                /* wait until BTC bit is set */
                while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
                i2c_write_process = I2C_TRANSMIT_DATA;
            }
            break;
        case I2C_TRANSMIT_DATA:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_TBE)) {
                /* the master sends a data byte */
                i2c_data_transmit(I2CX, *i2c_write++);
                i2c_nbytes--;
                if(RESET == i2c_nbytes) {
                    i2c_write_process = I2C_STOP;
                }
            }
            break;
        case I2C_STOP:
            /* the master sends a stop condition to I2C bus */
            i2c_stop_on_bus(I2CX);
            /* disable the I2CX interrupt */
            i2c_interrupt_disable(I2CX, I2C_INT_ERR);
            i2c_interrupt_disable(I2CX, I2C_INT_BUF);
            i2c_interrupt_disable(I2CX, I2C_INT_EV);
            i2c_write_process = I2C_SEND_ADDRESS_FIRST;
            break;
        default:
            break;
        }
    } else if(SET == i2c_process_flag) {
        switch(i2c_read_process) {
        case I2C_SEND_ADDRESS_FIRST:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_SBSEND)) {
                /* send slave address */
                i2c_master_addressing(I2CX, I2CX_SLAVE_ADDRESS7, I2C_TRANSMITTER);
                i2c_read_process = I2C_CLEAR_ADDRESS_FLAG_FIRST;
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG_FIRST:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_ADDSEND)) {
                /*clear ADDSEND bit */
                i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_ADDSEND);
                i2c_read_process = I2C_TRANSMIT_WRITE_READ_ADD;
            }
            break;
        case I2C_TRANSMIT_WRITE_READ_ADD:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_TBE)) {
                i2c_data_transmit(I2CX, i2c_read_dress);
                /* wait until BTC bit is set */
                while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
                i2c_stop_on_bus(I2CX);
                while(I2C_CTL0(I2CX) & I2C_CTL0_STOP);
                /* send a start condition to I2C bus */
                i2c_start_on_bus(I2CX);
                i2c_read_process = I2C_SEND_ADDRESS_SECOND;
            }
            break;
        case I2C_SEND_ADDRESS_SECOND:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_SBSEND)) {
                i2c_master_addressing(I2CX, I2CX_SLAVE_ADDRESS7, I2C_RECEIVER);
                if(i2c_nbytes < 3) {
                    /* disable acknowledge */
                    i2c_ack_config(I2CX, I2C_ACK_DISABLE);
                }
                i2c_read_process = I2C_CLEAR_ADDRESS_FLAG_SECOND;
            }
            break;
        case I2C_CLEAR_ADDRESS_FLAG_SECOND:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_ADDSEND)) {
                /*clear ADDSEND bit */
                i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_ADDSEND);
                if((1 == i2c_nbytes) || (2 == i2c_nbytes)) {
                    /* clear the ACKEN before the ADDSEND is cleared */
                    i2c_ack_config(I2CX, I2C_ACK_DISABLE);
                }
                i2c_read_process = I2C_TRANSMIT_DATA;
            }
            break;
        case I2C_TRANSMIT_DATA:
            if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_RBNE)) {
                if(i2c_nbytes > 0) {
                    if(i2c_nbytes == 3) {
                        /* wait until BTC bit is set */
                        while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
                        /* disable acknowledge */
                        i2c_ack_config(I2CX, I2C_ACK_DISABLE);
                    }
                    /* read a byte from the EEPROM */
                    *i2c_read = i2c_data_receive(I2CX);
                    i2c_read++;
                    i2c_nbytes--;
                    if(i2c_nbytes == 0) {
                        i2c_read_process = I2C_SEND_ADDRESS_FIRST;
                        /* the master sends a stop condition to I2C bus */
                        i2c_stop_on_bus(I2CX);
                        /* disable the I2CX interrupt */
                        i2c_interrupt_disable(I2CX, I2C_INT_ERR);
                        i2c_interrupt_disable(I2CX, I2C_INT_BUF);
                        i2c_interrupt_disable(I2CX, I2C_INT_EV);
                        i2c_process_flag = RESET;
                    }
                }
            }
            break;
        default:
            break;
        }
    }
}

/*!
    \brief      handle I2CX error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2cx_error_irq_handler(void)
{
    /* no acknowledge received */
    if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_AERR)) {
        i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_AERR);
    }

    /* SMBus alert */
    if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_SMBALT)) {
        i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_SMBALT);
    }

    /* bus timeout in SMBus mode */
    if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_SMBTO)) {
        i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_SMBTO);
    }

    /* over-run or under-run when SCL stretch is disabled */
    if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_OUERR)) {
        i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_OUERR);
    }

    /* arbitration lost */
    if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_LOSTARB)) {
        i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_LOSTARB);
    }

    /* bus error */
    if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_BERR)) {
        i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_BERR);
    }

    /* CRC value doesn't match */
    if(i2c_interrupt_flag_get(I2CX, I2C_INT_FLAG_PECERR)) {
        i2c_interrupt_flag_clear(I2CX, I2C_INT_FLAG_PECERR);
    }

    /* disable the I2CX interrupt */
    i2c_interrupt_disable(I2CX, I2C_INT_ERR);
    i2c_interrupt_disable(I2CX, I2C_INT_BUF);
    i2c_interrupt_disable(I2CX, I2C_INT_EV);
}



/*!
    \brief      this function handles I2C0 event interrupt request exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_EV_IRQHandler(void)
{
    i2cx_event_irq_handler();
}

/*!
    \brief      this function handles I2C0 error interrupt request exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C0_ER_IRQHandler(void)
{
    i2cx_error_irq_handler();
}



