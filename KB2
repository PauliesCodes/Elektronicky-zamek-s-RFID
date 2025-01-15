#define F_CPU 11059200
#define sda ibit (PORTA, 7)
#define scl ibit (PORTA, 6)
#define pwr ibit (PORTA, 0)
#define kbd_port PORTC

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h> 
#include <avr/sleep.h>
#include <stdio.h>

#include "W:\\School\\ROP\\INCLUDE\\lcd_i2c_u.h"
//#include "W:\\School\\ROP\\INCLUDE\\bit_mac_u.h"
#include "W:\\School\\ROP\\INCLUDE\\i2c_u.h"
#include "W:\\School\\ROP\\INCLUDE\\kbd_u.h"

#define MAX_CHIPS 10
#define STATUS_START_ADDR 10 
#define STATUS_ADDR_LENGHT 10 // 10,11,12,13,14,15,16,17,18,19
#define DATA_START_ADDR 20 // Vždy 16 20-35,36-51,.... 
#define PIN_START_ADDR 90
#define CHIP_LENGTH 16
#define PIN_LENGTH 7
#define TIME_TO_EXIT_MS 10000

volatile uint8_t data_prijata[16];
volatile uint8_t i = 0;
volatile bool start_OK = false;
volatile bool data_OK = false;
volatile bool stejny = false;
char pristupovyPin[7];

int idle_time = 0;

ISR(USART0_RX_vect)
{
	if(UDR0 == 0x02)
	{
		start_OK = true;
	}
	
	if (start_OK)
	{
		data_prijata[i] = UDR0;
		i++;
		
		if(i > 13)
		{
			data_OK = true;
			i = 0;
			start_OK = false;
		}
	}
}

ISR(TIMER1_COMPA_vect) {
	idle_time += 10;

}

bool timesUP(){
	
	if(idle_time / 100 >= TIME_TO_EXIT_MS) return true; 
	else return false;

}

//const char ulozenyKod[16] = "0112E9D2E8C0"; // muj do skoly cip xd
//const char ulozenyKod[16] = "1112E9D2E8C0";

void showMenuText(){

	text_row1("Zadejte pin");
	text_row2("Prilozte cip");

}

void FirstStartUp() {
	char code[7] = {0};
	uint8_t index = 0;
	char key;

	text_row1("Zadej kod:");

	while (1) {
		
		kb_on_timer1();

		if (KB_CMD1 & 0x80) {
			
			key = (char)(KB_CMD1 & 0x7F);
			KB_CMD1 = 0;

			if (key >= '0' && key <= '9' && index < 6) {
				
				code[index] = key;
				code[index + 1] = '\0';
				index++;
				text_row2(code);
				
				} else if (key == '*' && index > 0) {
				
				index--;
				code[index] = '\0';
				text_row2(code);
				
				} else if (key == 'D' && index == 6) {
				
				text_row1("Kod ulozen");
				_delay_ms(1000);
				
				for (int i = 0; i < 7; i++) {
					pristupovyPin[i] = code[i];
				}
				break;
			}
		}
	}
}



bool ComparePin(const char* zadanyPin) {

	if (strcmp(zadanyPin, pristupovyPin) == 0) return true;

	return false;
		
}

void SetRegistersInit(){

	UCSR0A = 0;
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //povoleni p?eru?en? od p??jmu
	UCSR0C |= (1<<UPM01)|(1<<UCSZ01)|(1<<UCSZ00);
	UBRR0 = 71; //9600

	DDRA = 0b11111111;
	PORTA= 0b10000000;

	DDRB = 255;
	PORTB = 255;
	
	kb_init();
	i2c_init();
	set_bit(prt(pwr));

	TCCR1B = (1 << WGM12);                // CTC mód
	TCCR1B |= (1 << CS12) | (1 << CS10);  // Předdělička 1024
	OCR1A = 108;                          // Hodnota pro 10 ms interval
	TIMSK1 = (1 << OCIE1A);               // Povolení přerušení na compare match

	sei();
	
	if (i2c_open_wr(HWA,0))
	{
		lcd_clr();
		lcd_on();
		cur_on();
		cur_blink_off();
		_delay_ms(1);
		i2c_stop();
	}

}

void OpenLock(){

	PORTB = 0;
	text_row1("Pristup povolen");
	text_row2("");
	_delay_ms(1000);
	PORTB = 255;
	showMenuText();

}

void AccessDenied(){

	text_row1("Pristup zamitnut");
	text_row2("");
	_delay_ms(1000);
	showMenuText();
}


unsigned char EEPROM_read(unsigned int uiAddress)
{
	while(EECR & (1<<EEPE))
	;
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	return EEDR;
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE))
	;
	/* Set up address and Data Registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
}


int findFreeSlot() {
	
	for (int i = 0; i < MAX_CHIPS; i++) {
		
		if (EEPROM_read(STATUS_START_ADDR + i) == 0xFF) { // 255 je volná (0xFF), 0 je obsazená (0x00)
			return i;
			
		}
	}
	
	return -1; // Když není žádné místo volné
	
}

void saveChip(char chip[16], int slot) {

	EEPROM_write(STATUS_START_ADDR + slot, 0x00);
	/*
	//Jen na test
	unsigned char status = EEPROM_read(STATUS_START_ADDR + 0);
	char debugText[16];
	sprintf(debugText, "Status: %02X", status);
	text_row2(debugText);
	_delay_ms(1000);
	*/
	
	
	int baseAddress = DATA_START_ADDR + slot * CHIP_LENGTH;
	
	for (int i = 0; i < CHIP_LENGTH; i++) {
		EEPROM_write(baseAddress + i, chip[i]);
	
	}
	
	text_row1(chip);
	_delay_ms(1000);

}

void savePin(const char chip[6]) {
	
	for (int i = 0; i < PIN_LENGTH; i++) {
		EEPROM_write(PIN_START_ADDR + i, chip[i]);
		
	}
	
}

int readChip(int slot, char buffer[16]) {
	
	if (slot < 0 || slot >= MAX_CHIPS) {
		
		return 2; // Mimo hranici
		
	}

	if (EEPROM_read(STATUS_START_ADDR + slot) == 0xFF) {
		
		return 1; // Na slotu není uložen chip
		
	}

	int baseAddress = DATA_START_ADDR + slot * CHIP_LENGTH;
	
	for (int i = 0; i < CHIP_LENGTH; i++) {
		
		buffer[i] = EEPROM_read(baseAddress + i);
		
	}

	buffer[CHIP_LENGTH - 1] = '\0';

	return 0; // Všechno v pořádku
	
}

int isChipStored(const char chip[16]) {
	
	char buffer[CHIP_LENGTH];

	for (int i = 0; i < MAX_CHIPS; i++) {
		
		if (readChip(i, buffer) && strcmp(buffer, chip) == 0) {
			
			return 1; // Čip nalezen
			
		}
	}

	return 0; // Čip nebyl nalezen
	
}

int AddChip() {
	
	idle_time = 0;

	int freeSLot = findFreeSlot();
	
	/*
	char textt[17];      
	snprintf(textt, sizeof(textt), "Free slot %d", freeSLot);
	text_row1(textt);
	_delay_ms(500);
	*/
	
	if(freeSLot == -1){
		return 0; //Prekrocen max pocet chipu

	}

	text_row1("Prilozte chip");
	text_row2("pro pridani");

	while (1) {

		if (data_OK) {

			char chip[16];

			for (int i = 0; i < 16; i++) {
				chip[i] = (char)data_prijata[i + 2];
			}

			data_OK = false;

			if (isChipStored(chip)) {
				
				return 3; // Indikace, že čip již existuje
				
			}

			saveChip(chip, freeSLot);
			
			return 2; // Vše ok

		}

		if(timesUP()) return 1; // Times out
		
	}
}

int RemoveChip() {
    idle_time = 0; 

	text_row1("Prilozte chip");
	text_row2("pro odebrani");

    while (1) {
		
        if (data_OK) {
			
            char chip[16];
			
            for (int i = 0; i < 16; i++) {
                chip[i] = (char)data_prijata[i + 2];
				
            }
			
            data_OK = false;

            for (int i = 0; i < MAX_CHIPS; i++) {
				
                char buffer[16];
				
                if (readChip(i, buffer) == 0 && strcmp(buffer, chip) == 0) {

                    EEPROM_write(STATUS_START_ADDR + i, 0xFF); 
                    
                    /* 
                    for (int j = 0; j < CHIP_LENGTH; j++) {
                        EEPROM_write(DATA_START_ADDR + i * CHIP_LENGTH + j, 0xFF); 
                    }
                    */

                    return 0; // Čip byl úspěšně odstraněn
                }
            }

            return 1; // Čip nebyl nalezen
        }

        if (timesUP()) {
            return 2; // Vypršel časový limit
			
        }
    }
}


bool CheckForOpenByRFID(bool &open){

	if(data_OK)
	{
		char text[16];

		for (int i = 0; i < 16; i++) {
			text[i] = (char)data_prijata[i + 2];
		}
		
		open = isChipStored(text);

		data_OK = false;

		return true;
	}

	return false;

}

int main(void)
{
	SetRegistersInit();	

	FirstStartUp();

	showMenuText();

	bool canOpen = false;
	char key;
	char zadanyPin[7] = {0};
	uint8_t pinIndex = 0;

	while (1)
	{
		
		if(CheckForOpenByRFID(canOpen)){

			if(canOpen) OpenLock();
			else AccessDenied();

		}

		kb_on_timer1();

		if (KB_CMD1 & 0x80){

			key = (char)(KB_CMD1 & 0x7F);
			KB_CMD1 = 0;

			if (key >= '0' && key <= '9' && pinIndex < 6) {

				zadanyPin[pinIndex] = key;
				zadanyPin[pinIndex + 1] = '\0';
				pinIndex++;
				text_row2(zadanyPin);
				
			} else if (key == '*' && pinIndex > 0) {

				pinIndex--;
				zadanyPin[pinIndex] = '\0';
				text_row2(zadanyPin);
				
			} else if (key == 'D' && pinIndex == 6) {
				
				if(ComparePin(zadanyPin)) OpenLock();
				else AccessDenied();

				pinIndex = 0;

			} else if(key == 'A') {

				int result = AddChip();

				if(result == 0) text_row1("No Space!");
				else if(result == 1) text_row1("Timed out!");
				else if (result == 2) text_row1("Success");
				else if (result == 3) text_row1("Is Added");
				_delay_ms(1000);
				showMenuText();

			} else if (key == 'B') {
				
				int result = RemoveChip();

				if (result == 0) text_row1("Chip removed!");
				else if (result == 1) text_row1("Not found!");
				else if (result == 2) text_row1("Timed out!");

				_delay_ms(1000);
				showMenuText();
				
			}
			
		}
	}
}

