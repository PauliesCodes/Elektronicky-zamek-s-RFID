#include <util/delay.h>
#include "bit_mac_u.h"

#ifndef i2c_wait
#define  i2c_wait 5
#endif

#ifndef sda
#define sda ibit(PORTA, 2)
#endif

#ifdef scl
#define scl ibit(PORTA, 1)
#endif

#define i2c_s_delay_us (1*i2c_wait)
#define i2c_l_delay_us (2*i2c_wait)

//***************************************

void i2c_init(void)
{
    
    set_bit(ddr(sda));
    set_bit(ddr(scl));
    set_bit(prt(sda));
    set_bit(prt(scl));
    
}

//**************************************

void i2c_start(void)
{
    
    set_bit(sda);
    i2c_s_delay_us;
    set_bit(scl);
    i2c_s_delay_us;
    clr_bit(sda);
    i2c_s_delay_us;
    
}

//*************************************

void i2c_stop(void){
    
    clr_bit(sda);
    i2c_s_delay_us;
    set_bit(scl);
    i2c_s_delay_us;
    set_bit(sda);
    i2c_s_delay_us;
    
}

//*************************************

uint8_t i2c_wr_byte(uint8_t value){
    
    uint8_t ACK = 0;
    
    for(uint8_t i = 0; i < 8; i++){
        
        if(value & 0x80){
            
            set_bit(sda);
                        
        }
        else {
            
            clr_bit(sda);
            
        }
        
        value = value << 1;
        
        i2c_s_delay_us;
        set_bit(scl);
        i2c_l_delay_us;
        clr_bit(scl);
        i2c_s_delay_us;
                
    }    
    
    rel_bit_x(sda);
    i2c_s_delay_us;
    set_bit(scl);
    i2c_s_delay_us;
    ACK = get_bit(pin(sda));
    i2c_s_delay_us;
    clr_bit(scl);
    i2c_s_delay_us;
    return(ACK);
    
}

//*************************************

uint8_t i2c_open_wr(uint8_t hw_addr, uint8_t sw_addr){
    
    i2c_start();
    
    if(i2c_wr_byte(hw_addr) == 0)
    {
        
        i2c_wr_byte(sw_addr);
        return(1);
        
    }
    
    return(0);
    
}