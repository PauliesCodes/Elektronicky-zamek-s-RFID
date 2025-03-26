/*
 * 595_u
 *
 * Created: 21.1.2018 11:00:00
 * Author: LH
 */ 

#ifndef _595_u_
#define _595_u_ = 1

#include <avr/io.h>
#include <stdio.h>
#include <ctype.h>
#include <util/delay.h>
#include "bit_mac_u.h"

#ifndef d59_port
#define d59_port PORTA
#endif

#ifndef d59_clk
#define d59_clk ibit(d59_port, 2)
#endif

#ifndef d59_dat
#define d59_dat ibit(d59_port, 0)
#endif

#ifndef d59_rwr
#define d59_rwr ibit(d59_port, 3)
#endif

#ifndef d59_enb
#define d59_enb ibit(d59_port, 1)
#endif

#ifndef d59_wait
#define d59_wait 1
#endif

#ifndef d59_digit
#define d59_digit 8
#endif

#define d59_delay _delay_us(d59_wait)

//***********************************************************************************************************************

char d59_buff[d59_digit + 1]; //+ #0

//***********************************************************************************************************************

void d5_init()
{
	bit_out_1(d59_clk);
	bit_out_1(d59_dat);
	bit_out_1(d59_rwr);
	bit_out_1(d59_enb);
}

//***********************************************************************************************************************

void d5_send_byte(uint8_t val)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		clr_bit(d59_clk);
		if (val & 0x01)
		{
			set_bit(d59_dat);
		}
		else
		{
			clr_bit(d59_dat);
		}
		d59_delay;	
		set_bit(prt(d59_clk));
		d59_delay;
		val = val >> 1;		
	}
}

//***********************************************************************************************************************

uint8_t d5_decode(char val)
{
	switch (toupper(val))
	{
		case '0':
			return(0B10010000);
		case '1':
			return(0B11010111);
		case '2':
			return(0B10100010);
		case '3':
			return(0B10000011);
		case '4':
			return(0B11000101);
		case '5':
			return(0B10001001);
		case '6':
			return(0B10001000);
		case '7':
			return(0B11010011);
		case '8':
			return(0B10000000);
		case '9':
			return(0B10000001);
		case '-':
			return(0B11101111);
		case 'A':
			return(0B11000000);
		case 'B':
			return(0B10001100);
		case 'C':
			return(0B10111000);
		case 'D':
			return(0B10000110);
		case 'E':
			return(0b10101000);
		case 'F':
			return(0B11101000);
		case 'H':
			return(0B11000100);
		case 'J':
			return(0B10010110);
		case 'L':
			return(0B10111100);
		case 'N':
			return(0B11001110);
		case 'O':
			return(0B10010000);
		case 'P':
			return(0B11100000);
		case 'T':
			return(0B10101100);
		case 'U':
			return(0B10010100);
		case ',':
			return(0B10111111);
		case '.':
			return(0B10111111);
		case '°':
			return(0B11100001);
		case '=':
		return(0B10111011);
		case '_':
		return(0B10111111);
		case '0'+'t':
			return(0B00010000);
		case '1'+'t':
			return(0B01010111);
		case '2'+'t':
			return(0B00100010);
		case '3'+'t':
			return(0B00000011);
		case '4'+'t':
			return(0B01000101);
		case '5'+'t':
			return(0B00001001);
		case '6'+'t':
			return(0B00001000);
		case '7'+'t':
			return(0B01010011);
		case '8'+'t':
			return(0B00000000);
		case '9'+'t':
			return(0B00000001);			
		default:
			return(0B11111111);
	}
}
//***********************************************************************************************************************

void d5_update(void)
{
	clr_bit(d59_rwr);
	for (uint8_t i = 0; i < d59_digit; i++)
	{
		d5_send_byte(d5_decode(d59_buff[(d59_digit - 1) - i]));
	}
	set_bit(d59_rwr);
	clr_bit(d59_enb);
}

//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************

#ifdef __cplusplus

void d5_printf(const char *__fmt)
{
	snprintf(d59_buff, d59_digit + 1, __fmt);
	d5_update();
}

void d5_printf(const char *__fmt, const int p1)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1);
	d5_update();
}

void d5_printf(const char *__fmt, const int p1, const int p2)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2);
	d5_update();
}

void d5_printf(const char *__fmt, const int p1, const int p2, const int p3)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3);
	d5_update();
}

void d5_printf(const char *__fmt, const int p1, const int p2, const int p3, const int p4)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4);
	d5_update();
}

void d5_printf(const char *__fmt, const int p1, const int p2, const int p3, const int p4, const int p5, const int p6)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4, p5, p6);
	d5_update();
}

void d5_printf(const char *__fmt, const int p1, const int p2, const int p3, const int p4, const int p5, const int p6, const int p7)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4, p5, p6, p7);
	d5_update();
}

void d5_printf(const char *__fmt, const int p1, const int p2, const int p3, const int p4, const int p5, const int p6, const int p7, const int p8)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4, p5, p6, p7, p8);
	d5_update();
}

//
void d5_printf(const char *__fmt, const long p1)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1);
	d5_update();
}
//
void d5_printf(const char *__fmt, const float p1)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1);
	d5_update();
}

void d5_printf(const char *__fmt, const float p1, const float p2)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1, const char p2)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1, const char p2, const char p3)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1, const char p2, const char p3, const char p4)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1, const char p2, const char p3, const char p4, const char p5)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4, p5);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1, const char p2, const char p3, const char p4, const char p5, const char p6)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4, p5, p6);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1, const char p2, const char p3, const char p4, const char p5, const char p6, const char p7)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4, p5, p6, p7);
	d5_update();
}

void d5_printf(const char *__fmt, const char p1, const char p2, const char p3, const char p4, const char p5, const char p6, const char p7, const char p8)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4, p5, p6, p7, p8);
	d5_update();
}

//***********************************************************************************************************************

#else

//***********************************************************************************************************************

void d5_printf_c1(const char *__fmt, const char p1)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1);
	d5_update();
}

void d5_printf_c2(const char *__fmt, const char p1, const char p2)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2);
	d5_update();
}

void d5_printf_c3(const char *__fmt, const char p1, const char p2, const char p3)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3);
	d5_update();
}

void d5_printf_c4(const char *__fmt, const char p1, const char p2, const char p3, const char p4)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4);
	d5_update();
}

//***********************************************************************************************************************

void d5_printf_str(const char *__fmt)
{
	snprintf(d59_buff, d59_digit + 1, __fmt);
	d5_update();
}

//***********************************************************************************************************************

void d5_printf_i1(const char *__fmt, const int p1) 
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1);
	d5_update();
}

void d5_printf_i2(const char *__fmt, const int p1, const int p2)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2);
	d5_update();
}

void d5_printf_i3(const char *__fmt, const int p1, const int p2, const int p3)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3);
	d5_update();
}

void d5_printf_i4(const char *__fmt, const int p1, const int p2, const int p3, const int p4)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1, p2, p3, p4);
	d5_update();
}

//***********************************************************************************************************************

void d5_printf_f1(const char *__fmt, const float p1)
{
	snprintf(d59_buff, d59_digit + 1, __fmt, p1);
	d5_update();
}

//***********************************************************************************************************************

#endif
#endif