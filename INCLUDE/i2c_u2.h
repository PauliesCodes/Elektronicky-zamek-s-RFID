/*
 * i2c_u.h
 *
 * Created: 10.1.2018 11:47:45
 * Author: LH
 */ 


/* 
	i2c_init2();

	if (i2c_open_rd2(0b11010000, 0)) //hw_adresa, sw_adresa 
	{
		val1 = i2c_rd_byte2(0);
		val2 = i2c_rd_byte2(1);
		i2c_stop2();
	}

    if (i2c_open_wr2(0b11010000, 0)) //hw_adresa, sw_adresa
    {
	    i2c_wr_byte2(0x11);
	    i2c_wr_byte2(0x22);
	    i2c_stop2();
    }
*/

#ifndef _i2c_u_2
#define _i2c_u_2 1

#include <avr/io.h>
#include <util/delay.h>
#include "bit_mac_u.h"

#ifndef i2c_wait2
#define i2c_wait2 5
#endif

#ifndef sda2 
#define sda2 ibit (PORTC, 2)
#endif

#ifndef scl2 
#define scl2 ibit (PORTC, 1)
#endif

#define i2c_s_delay2 _delay_us(1 * i2c_wait2)	//kratke zpozdeni
#define i2c_l_delay2 _delay_us(2 * i2c_wait2)	//dlouhe zpozdeni

//***********************************************************************************************************************

void i2c_init2(void)
{
	clr_bit(ddr(scl2));
	clr_bit(ddr(sda2));
	set_bit(prt(scl2)); 
	set_bit(prt(sda2)); 
}

//***********************************************************************************************************************

void i2c_start2(void)
{
	set_bit_x(sda2); //SDA do 1
	i2c_s_delay2;
	set_bit_x(scl2); //SCL do 1
	i2c_s_delay2;
	clr_bit_x(sda2); //SDA do 0
	i2c_s_delay2;
	clr_bit_x(scl2); //SCL do 0
	i2c_s_delay2;
}

//***********************************************************************************************************************

void i2c_stop2(void)
{
	clr_bit_x(sda2); //SDA do 0
	i2c_s_delay2;
	set_bit_x(scl2); //SCL do 1
	i2c_s_delay2;
	set_bit_x(sda2); //SDA do 1
	i2c_s_delay2;
}

//***********************************************************************************************************************

uint8_t i2c_wr_byte2(uint8_t value)
{
	uint8_t result = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		if ((value & 0x80) == 0)
		{
			clr_bit_x(sda2);
		}
		else
		{
			set_bit_x(sda2);
		}
		value = value << 1;
		i2c_s_delay2;
		set_bit_x(scl2);
		i2c_l_delay2;
		clr_bit_x(scl2);
		i2c_s_delay2;
	}
	rel_bit_x(sda2);
	i2c_s_delay2;
	set_bit_x(scl2); //SCL do 1, clock pro ACK
	i2c_s_delay2;
	result = get_bit(pin(sda2));
    i2c_s_delay2;
	clr_bit_x(scl2);
	i2c_s_delay2;
	return(result);
} 

//***********************************************************************************************************************

uint8_t i2c_rd_byte2(uint8_t ack) //ack je 1 az u posledniho cteneho bytu, jinak 0
{
	uint8_t result = 0;
	
	rel_bit_x(sda2);
	for (uint8_t i = 0; i < 8; i++)
	{
		result = result << 1;
		i2c_s_delay2;
		set_bit_x(scl2);
		i2c_s_delay2;
		if (get_bit(pin(sda2)))
		{
		  result |= 0x01;
		}
		i2c_s_delay2;
		clr_bit_x(scl2);
		i2c_s_delay2;
	}
	if (ack == 0)
	{
		clr_bit_x(sda2);
	}
	else
	{
		set_bit_x(sda2);
	}
	i2c_s_delay2;
	set_bit_x(scl2);
	i2c_l_delay2;
	clr_bit_x(scl2);
	i2c_s_delay2;
	return(result);
}
	
//***********************************************************************************************************************

uint8_t i2c_open_wr2(uint8_t hw_addr, uint8_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start2();
		if (i2c_wr_byte2(hw_addr) == 0)
		{
			i2c_wr_byte2(sw_addr);
			return(1);
		}
		i2c_stop2();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_wr2_162(uint8_t hw_addr, uint16_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start2();
		if (i2c_wr_byte2(hw_addr) == 0)
		{
			i2c_wr_byte2((sw_addr >> 8) & 0xFF);
			i2c_wr_byte2((sw_addr >> 0) & 0xFF);
			return(1);
		}
		i2c_stop2();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_rd2(uint8_t hw_addr, uint8_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start2();
		if (i2c_wr_byte2(hw_addr) == 0)
		{
			i2c_wr_byte2(sw_addr);
			i2c_start2();
			i2c_wr_byte2(hw_addr + 1);
			return(1);
		}
		i2c_stop2();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_rd2_162(uint8_t hw_addr, uint16_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start2();
		if (i2c_wr_byte2(hw_addr) == 0)
		{
			i2c_wr_byte2((sw_addr >> 8) & 0xFF);
			i2c_wr_byte2((sw_addr >> 0) & 0xFF);
			i2c_start2();
			i2c_wr_byte2(hw_addr + 1);
			return(1);
		}
		i2c_stop2();
	}
	return(0);
}

//***********************************************************************************************************************

#endif 