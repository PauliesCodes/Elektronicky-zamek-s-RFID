/*
 * kbd_u
 *
 * Created: 10.1.2018 11:47:45
 * Author: LH
 */ 

#ifndef _kbd_u_
#define _kbd_u_ = 1

#include "bit_mac_u.h"

#ifndef kbd_port
#define kbd_port PORTA
#endif

#define	KB_FRRP 600	   										//autorepeat 1-2 (ms)
#define	KB_NXRP 150											//autorepeat 2-X (ms)
#define	KB_FILT 20											//filtr zakmitu (ms)

//***********************************************************************************************************************

volatile uint8_t KB_CMD1;
volatile uint8_t KB_CMD2;
volatile uint8_t KB_CNTR;
volatile uint8_t KB_LAST;

//***********************************************************************************************************************

void kb_init()
{
	PRT(kbd_port) = 0xFF;
	DDR(kbd_port) = 0X00;
}

//***********************************************************************************************************************

uint8_t kb_read_row(uint8_t mask) //cteni radku
{
	uint8_t result;
	
	DDR(kbd_port) = 0XFF;
	PRT(kbd_port) = ~mask;
	DDR(kbd_port) = +mask;
	asm volatile("nop \n\t");
	asm volatile("nop \n\t");
	result = PIN(kbd_port) & 0x0F;
	PRT(kbd_port) = 0xFF;
	DDR(kbd_port) = 0XFF;
	DDR(kbd_port) = 0X00;
	return(result);
}

//***********************************************************************************************************************

uint8_t kb_decode_row(uint8_t m0, uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4)
{
	uint8_t result;

	switch (kb_read_row(m0))
	{
		case 0x0E:
			result = m1;
			break;
		case 0x0D:
			result = m2;
			break;
		case 0x0B:
			result = m3;
			break;
		case 0x07:
			result = m4;
			break;
		case 0x0F: 
			result = 0x00; //zadna
			break;
		default:		
			result = 0XFF; //neplatna
			break;	
	}
	return(result);
}

//***********************************************************************************************************************

uint8_t kb_read_port(void)
{
	uint8_t result, temp, rowcnt;
	
	rowcnt = 0;
	result = 0;

	temp = kb_decode_row(0x10, '1', '2', '3', 'A');
	if (temp)
	{
		rowcnt++;
		if (temp != 255)
		{
			result = temp;
		}
	}
	
	temp = kb_decode_row(0x20, '4', '5', '6', 'B');
	if (temp)
	{
		rowcnt++;
		if (temp != 255)
		{
			result = temp;
		}
	}
	
	temp = kb_decode_row(0x40, '7', '8', '9', 'C');
	if (temp)
	{
		rowcnt++;
		if (temp != 255)
		{
			result = temp;
		}
	}
	
	temp = kb_decode_row(0x80, '*', '0', '#', 'D');
	if (temp)
	{
		rowcnt++;
		if (temp != 255)
		{
			result = temp;
		}
	}
	
	if (rowcnt == 1) //prave na jednom radku
	{
		return(result);
	}
	else
	{
		return(0);
	}
}

//***********************************************************************************************************************

volatile uint16_t KB_DEL1;

void kb_on_timer1()
{
	uint8_t KB_KEYB = kb_read_port();
	
	if (KB_KEYB == KB_LAST)
	{
		if (KB_CNTR < KB_FILT)
		{
			KB_CNTR++;
			return;
		}
	}
	else
	{
		KB_LAST = KB_KEYB;
		KB_CNTR = 0;
		return;
	}
	
	if (KB_KEYB)
	{
		if (KB_DEL1)
		{
			if (--KB_DEL1 == 0)
			{
				if (KB_CMD2 == 0)
				{
					KB_CMD2 = KB_KEYB | 0x80;
				}	
				KB_DEL1 = KB_NXRP; 
			}
		}
		else
		{
			if (KB_CMD1 == 0)
			{
				KB_CMD1 = KB_KEYB | 0x80;
			}
			if (KB_CMD2 == 0)
			{
				KB_CMD2 = KB_KEYB | 0x80;
			}
			KB_DEL1 = KB_FRRP;		
		}
	}
	else
	{
		KB_DEL1 = 0;
	}
}

//***********************************************************************************************************************

#endif


