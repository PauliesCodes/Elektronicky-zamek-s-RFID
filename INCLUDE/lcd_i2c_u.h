// Jednotka pro pr�ci s lcd pomoc� sb�rnice i2c


#define HWA	0b01111000
#include "i2c_u.h"

//************************************************************
void lcd_clr()					//vymaz�n� displeje
{
	_delay_ms(1);
	i2c_wr_byte(0b11000000);	
	i2c_wr_byte(0b00000001);
	_delay_ms(1);	
}
//************************************************************
void lcd_on()					//zapnut� displeje 
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001100);
	_delay_ms(1);
	i2c_wr_byte(0b00000110);	// posuv kurzoru
	_delay_ms(1);
	i2c_wr_byte(0b00101000);	// dva ��dky
	_delay_ms(1);
}

//**************************************************
void lcd_off()					//vypnut� displeje 
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001000);
	_delay_ms(1);
}	

//************************************************************
void cur_on()					//zapnut� kurzoru 
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001110);
	_delay_ms(1);
}

//**************************************************
void cur_off()					//vypnut� kurzoru
{
	i2c_wr_byte(0b11000000);	
	_delay_ms(1);
	i2c_wr_byte(0b00001100);
	_delay_ms(1);
}	

//************************************************************
void cur_blink_on()					//zapnut� blik�n� kurzoru
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b00001111);
	_delay_ms(1);
}
//************************************************************
void cur_blink_off()					//vypnut� blik�n� kurzoru
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b00001110);
	_delay_ms(1);
}

//********************************************************
void set_row1_poz(uint8_t r)					//nastaven� ��dku 1 + pozice
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b10000000+r);
	_delay_ms(1);
}

//********************************************************
void set_row2_poz(uint8_t r)					//nastaven� ��dku 2 + pozice
{
	i2c_wr_byte(0b11000000);
	_delay_ms(1);
	i2c_wr_byte(0b11000000+r);
	_delay_ms(1);
}
//********************************************************
void text_row1(char t[])					//v�pis textu na ��dek 1
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
void text_row2(char t[])					//v�pis textu na ��dek 2
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