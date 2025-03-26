/*
#ifndef bity_u
#define bity_u

#include <avr/io.h>
#include <util/delay.h>

const uint8_t maska_bitu[8]={0b11111110,0b11111101,0b11111011,0b11110111,0b11101111,0b11011111,0b10111111,0b01111111};

//*********************************************************
int bit_u(int pt,int b)
{
	if (pt == (pt & maska_bitu[b]))
	{
		return(0);
	} 
	else
	{
		return(1);
	}
}

//****************************************
#define clr_bit_u(pt,b) pt = pt & maska_bitu[b]

//****************************************
#define set_bit_u(pt,b) pt = pt | ~maska_bitu[b]

//****************************************
#define cpl_bit_u(pt,b) pt = pt ^ ~maska_bitu[b]

//****************************************
#define copy_bit_u(ptc,bc,ptz,bz) if (bit_u(ptz,bz)) {set_bit_u(ptc,bc);} else {clr_bit_u(ptc,bc);}

//****************************************
#define pulse_up_u(pt,b,t) set_bit_u(pt,b); _delay_us(t); clr_bit_u(pt,b);

#endif
*/

#ifndef bity_u
#define bity_u

#include <avr/io.h>
#include <util/delay.h>

bool bit_u(int pt,int b) { //urcuje zda na vstupu je log. 0
	if (pt == (pt & ~(1 << b))) { return false; } 
	else { return true; }
}

#define clr_bit_u(pt,b) pt &= ~(1 << b) //nastavi bit na log.0
#define set_bit_u(pt,b) pt |= (1 << b) //nastavi bit na log.1
#define cpl_bit_u(pt,b) pt ^= (1 << b) //neguje bit
#define copy_bit_u(ptc,bc,ptz,bz) if (bit_u(ptz,bz)) { set_bit_u(ptc,bc); } else { clr_bit_u(ptc,bc); }
#define pulse_up_u(pt,b,t) set_bit_u(pt,b); _delay_us(t); clr_bit_u(pt,b);
#define pulse_down_u(pt,b,t) clr_bit_u(pt,b); _delay_us(t); set_bit_u(pt,b);
#endif