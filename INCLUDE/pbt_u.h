/*
 * pbt_u
 *
 * Created: 10.1.2018 11:47:45
 * Author: LH
 */ 

#ifndef _pbt_
#define _pbt_ = 1

#include "bit_mac_u.h"

#ifndef pbt_port
#define pbt_port PORTA
#endif

#define	PB_FRRP 600	   										//autorepeat 1-2 (ms)
#define	PB_NXRP 150											//autorepeat 2-X (ms)
#define	PB_FILT 20											//filtr zakmitu (ms)

//***********************************************************************************************************************

volatile uint8_t PB_CMD1;
volatile uint8_t PB_CMD2;
volatile uint8_t PB_CNTR;
volatile uint8_t PB_LAST;

//***********************************************************************************************************************

void pb_init()
{
	PRT(pbt_port) = 0xFF;
	DDR(pbt_port) = 0X00;
}

//***********************************************************************************************************************

uint8_t pb_read_port(void)
{	
	switch PIN(pbt_port)
	{
		case 0xFE:
			return('0');
		case 0xFD:
			return('1');
		case 0xFB:
			return('2');
		case 0xF7:
			return('3');
		case 0xEF:
			return('4');
		case 0xDF:
			return('5');
		case 0xBF:
			return('6');
		case 0x7F:
			return('7');
	}
	return(0);
}

//***********************************************************************************************************************

volatile uint16_t PB_DEL1;

void pb_on_timer1()
{
	uint8_t PB_KEYB = pb_read_port();
	
	if (PB_KEYB == PB_LAST)
	{
		if (PB_CNTR < PB_FILT)
		{
			PB_CNTR++;
			return;
		}
	}
	else
	{
		PB_LAST = PB_KEYB;
		PB_CNTR = 0;
		return;
	}
	
	if (PB_KEYB)
	{
		if (PB_DEL1)
		{
			if (--PB_DEL1 == 0)
			{
				if (PB_CMD2 == 0)
				{
					PB_CMD2 = PB_KEYB | 0x80;
				}
				PB_DEL1 = PB_NXRP; 
			}
		}
		else
		{
			if (PB_CMD1 == 0)
			{
				PB_CMD1 = PB_KEYB | 0x80;
			}
			if (PB_CMD2 == 0)
			{
				PB_CMD2 = PB_KEYB | 0x80;
			}
			PB_DEL1 = PB_FRRP;		
		}
	}
	else
	{
		PB_DEL1 = 0;
	}
}

//***********************************************************************************************************************

#endif


