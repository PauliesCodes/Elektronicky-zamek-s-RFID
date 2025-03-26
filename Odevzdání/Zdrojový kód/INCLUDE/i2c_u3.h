/*
 * i2c_u.h
 *
 * Created: 10.1.2018 11:47:45
 * Author: LH
 */ 


/* 
	i2c_init3();

	if (i2c_open_rd3(0b11010000, 0)) //hw_adresa, sw_adresa 
	{
		val1 = i2c_rd_byte3(0);
		val2 = i2c_rd_byte3(1);
		i2c_stop3();
	}

    if (i2c_open_wr3(0b11010000, 0)) //hw_adresa, sw_adresa
    {
	    i2c_wr_byte3(0x11);
	    i2c_wr_byte3(0x22);
	    i2c_stop3();
    }
*/

#ifndef _i2c_u_3
#define _i2c_u_3 1

#include <avr/io.h>
#include <util/delay.h>
#include "bit_mac_u.h"

#ifndef i2c_wait3
#define i2c_wait3 5
#endif

#ifndef sda3 
#define sda3 ibit (PORTC, 2)
#endif

#ifndef scl3 
#define scl3 ibit (PORTC, 1)
#endif

#define i2c_s_delay3 _delay_us(1 * i2c_wait3)	//kratke zpozdeni
#define i2c_l_delay3 _delay_us(2 * i2c_wait3)	//dlouhe zpozdeni

//***********************************************************************************************************************

void i2c_init3(void)
{
	clr_bit(ddr(scl3
));
	clr_bit(ddr(sda3));
	set_bit(prt(scl3
)); 
	set_bit(prt(sda3)); 
}

//***********************************************************************************************************************

void i2c_start3(void)
{
	set_bit_x(sda3); //SDA do 1
	i2c_s_delay3;
	set_bit_x(scl3
); //SCL do 1
	i2c_s_delay3;
	clr_bit_x(sda3); //SDA do 0
	i2c_s_delay3;
	clr_bit_x(scl3
); //SCL do 0
	i2c_s_delay3;
}

//***********************************************************************************************************************

void i2c_stop3(void)
{
	clr_bit_x(sda3); //SDA do 0
	i2c_s_delay3;
	set_bit_x(scl3
); //SCL do 1
	i2c_s_delay3;
	set_bit_x(sda3); //SDA do 1
	i2c_s_delay3;
}

//***********************************************************************************************************************

uint8_t i2c_wr_byte3(uint8_t value)
{
	uint8_t result = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		if ((value & 0x80) == 0)
		{
			clr_bit_x(sda3);
		}
		else
		{
			set_bit_x(sda3);
		}
		value = value << 1;
		i2c_s_delay3;
		set_bit_x(scl3
	);
		i2c_l_delay3;
		clr_bit_x(scl3
	);
		i2c_s_delay3;
	}
	rel_bit_x(sda3);
	i2c_s_delay3;
	set_bit_x(scl3
); //SCL do 1, clock pro ACK
	i2c_s_delay3;
	result = get_bit(pin(sda3));
    i2c_s_delay3;
	clr_bit_x(scl3
);
	i2c_s_delay3;
	return(result);
} 

//***********************************************************************************************************************

uint8_t i2c_rd_byte3(uint8_t ack) //ack je 1 az u posledniho cteneho bytu, jinak 0
{
	uint8_t result = 0;
	
	rel_bit_x(sda3);
	for (uint8_t i = 0; i < 8; i++)
	{
		result = result << 1;
		i2c_s_delay3;
		set_bit_x(scl3
	);
		i2c_s_delay3;
		if (get_bit(pin(sda3)))
		{
		  result |= 0x01;
		}
		i2c_s_delay3;
		clr_bit_x(scl3
	);
		i2c_s_delay3;
	}
	if (ack == 0)
	{
		clr_bit_x(sda3);
	}
	else
	{
		set_bit_x(sda3);
	}
	i2c_s_delay3;
	set_bit_x(scl3
);
	i2c_l_delay3;
	clr_bit_x(scl3
);
	i2c_s_delay3;
	return(result);
}
	
//***********************************************************************************************************************

uint8_t i2c_open_wr3(uint8_t hw_addr, uint8_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start3();
		if (i2c_wr_byte3(hw_addr) == 0)
		{
			i2c_wr_byte3(sw_addr);
			return(1);
		}
		i2c_stop3();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_wr3_162(uint8_t hw_addr, uint16_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start3();
		if (i2c_wr_byte3(hw_addr) == 0)
		{
			i2c_wr_byte3((sw_addr >> 8) & 0xFF);
			i2c_wr_byte3((sw_addr >> 0) & 0xFF);
			return(1);
		}
		i2c_stop3();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_rd3(uint8_t hw_addr, uint8_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start3();
		if (i2c_wr_byte3(hw_addr) == 0)
		{
			i2c_wr_byte3(sw_addr);
			i2c_start3();
			i2c_wr_byte3(hw_addr + 1);
			return(1);
		}
		i2c_stop3();
	}
	return(0);
}

//***********************************************************************************************************************

uint8_t i2c_open_rd3_162(uint8_t hw_addr, uint16_t sw_addr)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		i2c_start3();
		if (i2c_wr_byte3(hw_addr) == 0)
		{
			i2c_wr_byte3((sw_addr >> 8) & 0xFF);
			i2c_wr_byte3((sw_addr >> 0) & 0xFF);
			i2c_start3();
			i2c_wr_byte3(hw_addr + 1);
			return(1);
		}
		i2c_stop3();
	}
	return(0);
}

//***********************************************************************************************************************

#endif 