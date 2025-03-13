/*
 * i2c_u.h
 *
 * Created: 10.1.2018 11:47:45
 * Author: LH
 */ 


/* 
	i2c_init();

	if (i2c_open_rd(0b11010000, 0)) //hw_adresa, sw_adresa 
	{
		val1 = i2c_rd_byte(0);
		val2 = i2c_rd_byte(1);
		i2c_stop();
	}

    if (i2c_open_wr(0b11010000, 0)) //hw_adresa, sw_adresa
    {
	    i2c_wr_byte(0x11);
	    i2c_wr_byte(0x22);
	    i2c_stop();
    }
*/

#ifndef _i2c_u_
#define _i2c_u_ 1

#include <avr/io.h>
#include <util/delay.h>
#include "bit_mac_u.h"

#ifndef i2c_wait
#define i2c_wait 5
#endif

#ifndef sda 
#define sda ibit (PORTC, 2)
#endif

#ifndef scl 
#define scl ibit (PORTC, 1)
#endif

#define i2c_s_delay _delay_us(1 * i2c_wait)	//kratke zpozdeni
#define i2c_l_delay _delay_us(2 * i2c_wait)	//dlouhe zpozdeni

//***********************************************************************************************************************

void i2c_init(void)
{
	clr_bit(ddr(scl));
	clr_bit(ddr(sda));
	set_bit(prt(scl)); 
	set_bit(prt(sda)); 
}

//***********************************************************************************************************************

void i2c_start(void)
{
	set_bit_x(sda); //SDA do 1
	i2c_s_delay;
	set_bit_x(scl); //SCL do 1
	i2c_s_delay;
	clr_bit_x(sda); //SDA do 0
	i2c_s_delay;
	clr_bit_x(scl); //SCL do 0
	i2c_s_delay;
}

//***********************************************************************************************************************

void i2c_stop(void)
{
	clr_bit_x(sda); //SDA do 0
	i2c_s_delay;
	set_bit_x(scl); //SCL do 1
	i2c_s_delay;
	set_bit_x(sda); //SDA do 1
	i2c_s_delay;
}

//***********************************************************************************************************************

uint8_t i2c_wr_byte(uint8_t value)
{
	uint8_t result = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		if ((value & 0x80) == 0)
		{
			clr_bit_x(sda);
		}
		else
		{
			set_bit_x(sda);
		}
		value = value << 1;
		i2c_s_delay;
		set_bit_x(scl);
		i2c_l_delay;
		clr_bit_x(scl);
		i2c_s_delay;
	}
	rel_bit_x(sda);
	i2c_s_delay;
	set_bit_x(scl); //SCL do 1, clock pro ACK
	i2c_s_delay;
	result = get_bit(pin(sda));
    i2c_s_delay;
	clr_bit_x(scl);
	i2c_s_delay;
	return(result);
} 

//***********************************************************************************************************************

uint8_t i2c_rd_byte(uint8_t ack) //ack je 1 az u posledniho cteneho bytu, jinak 0
{
	uint8_t result = 0;
	
	rel_bit_x(sda);
	for (uint8_t i = 0; i < 8; i++)
	{
		result = result << 1;
		i2c_s_delay;
		set_bit_x(scl);
		i2c_s_delay;
		if (get_bit(pin(sda)))
		{
		  result |= 0x01;
		}
		i2c_s_delay;
		clr_bit_x(scl);
		i2c_s_delay;
	}
	if (ack == 0)
	{
		clr_bit_x(sda);
	}
	else
	{
		set_bit_x(sda);
	}
	i2c_s_delay;
	set_bit_x(scl);
	i2c_l_delay;
	clr_bit_x(scl);
	i2c_s_delay;
	return(result);
}
	
//***********************************************************************************************************************

uint8_t i2c_open_wr(uint8_t hw_addr, uint8_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start();
		if (i2c_wr_byte(hw_addr) == 0)
		{
			i2c_wr_byte(sw_addr);
			return(1);
		}
		i2c_stop();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_wr_16(uint8_t hw_addr, uint16_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start();
		if (i2c_wr_byte(hw_addr) == 0)
		{
			i2c_wr_byte((sw_addr >> 8) & 0xFF);
			i2c_wr_byte((sw_addr >> 0) & 0xFF);
			return(1);
		}
		i2c_stop();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_rd(uint8_t hw_addr, uint8_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start();
		if (i2c_wr_byte(hw_addr) == 0)
		{
			i2c_wr_byte(sw_addr);
			i2c_start();
			i2c_wr_byte(hw_addr + 1);
			return(1);
		}
		i2c_stop();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_rd_16(uint8_t hw_addr, uint16_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start();
		if (i2c_wr_byte(hw_addr) == 0)
		{
			i2c_wr_byte((sw_addr >> 8) & 0xFF);
			i2c_wr_byte((sw_addr >> 0) & 0xFF);
			i2c_start();
			i2c_wr_byte(hw_addr + 1);
			return(1);
		}
		i2c_stop();
	}
	return(0);
}

//***********************************************************************************************************************

#endif 