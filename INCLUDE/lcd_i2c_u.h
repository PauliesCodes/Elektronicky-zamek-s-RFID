// Jednotka pro práci s lcd pomocí sbìrnice i2c


#define HWA	0b01111000
#include "i2c_u.h"

//************************************************************
void lcd_clr()					//vymazání displeje
{
	_delay_ms(1);
	i2c_wr_byte(0b11000000);	
	i2c_wr_byte(0b00000001);
	_delay_ms(1);	
}
//************************************************************
void lcd_on()					//zapnutí displeje 
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001100);
	_delay_ms(1);
	i2c_wr_byte(0b00000110);	// posuv kurzoru
	_delay_ms(1);
	i2c_wr_byte(0b00101000);	// dva øádky
	_delay_ms(1);
}

//**************************************************
void lcd_off()					//vypnutí displeje 
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001000);
	_delay_ms(1);
}	

//************************************************************
void cur_on()					//zapnutí kurzoru 
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001110);
	_delay_ms(1);
}

//**************************************************
void cur_off()					//vypnutí kurzoru
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001100);
	_delay_ms(1);
}	

//************************************************************
void cur_blink_on()					//zapnutí blikání kurzoru
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b00001111);
	_delay_ms(1);
}
//************************************************************
void cur_blink_off()					//vypnutí blikání kurzoru
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b00001110);
	_delay_ms(1);
}

//********************************************************
void set_row1_poz(uint8_t r)					//nastavení øádku 1 + pozice
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b10000000+r);
	_delay_ms(1);
}

//********************************************************
void set_row2_poz(uint8_t r)					//nastavení øádku 2 + pozice
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b11000000+r);
	_delay_ms(1);
}
//********************************************************
void text_row1(char t[])					//výpis textu na øádek 1
{
	char lcd_buff[17] = "                ";
	
	if (i2c_open_wr(HWA,0))
	{
		set_row1_poz(0);
		_delay_ms(1);
		i2c_stop();
	}

	for (unsigned int i = 0; i<strlen(t);i++)
	{
		lcd_buff[i] = t[i];
	}
	
	if (i2c_open_wr(HWA,0x41))
	{
		for (uint8_t i =0; i< 16;i++)
		{
			i2c_wr_byte(lcd_buff[i]);
		}
		_delay_ms(1);
		i2c_stop();
	}
}
//********************************************************
void text_row2(char t[])					//výpis textu na øádek 2
{
	char lcd_buff[17] = "                ";
	
	if (i2c_open_wr(HWA,0))
	{
		set_row2_poz(0);
		_delay_ms(1);
		i2c_stop();
	}

	for (unsigned int i = 0; i<strlen(t);i++)
	{
		lcd_buff[i] = t[i];
	}
	
	if (i2c_open_wr(HWA,0x41))
	{
		for (uint8_t i =0; i< 16;i++)
		{
			i2c_wr_byte(lcd_buff[i]);
		}
		_delay_ms(1);
		i2c_stop();
	}
}
//********************************************************