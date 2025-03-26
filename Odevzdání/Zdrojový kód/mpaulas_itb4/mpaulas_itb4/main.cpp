#define F_CPU 11059200
#define sda ibit (PORTA, 7)
#define scl ibit (PORTA, 6)
#define pwr ibit (PORTA, 0)
#define kbd_port PORTC

// PORTA LCD
// PORTB Ledky
// PORTC KeyPad
// PORTD RFID


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <avr/wdt.h>

#include "..//..//INCLUDE/lcd_i2c_u.h"
#include "..//..//INCLUDE/i2c_u.h"
#include "..//..//INCLUDE/kbd_u.h"

#define MAX_CHIPS 10
#define STATUS_START_ADDR 10
#define STATUS_ADDR_LENGHT 10
#define DATA_START_ADDR 20
#define FIRST_STARTUP 0
#define PIN_START_ADDR 1
#define CHIP_LENGTH 16
#define PIN_LENGTH 7
#define TIME_TO_EXIT_MS 4000

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

void startTimer(){
	
	idle_time = 0;
	
}

bool timesUP(){
	return (idle_time >= TIME_TO_EXIT_MS);
}

void showMenuText(){

	text_row1("Enter PIN");
	text_row2("Enclose CHIP");

}

void clearDisplay(){

	text_row1("");
	text_row2("");

}

void savePin(const char pin[6]) {
	
	for (int i = 0; i < PIN_LENGTH; i++) {
		EEPROM_write(PIN_START_ADDR + i, pin[i]);
		
	}
	
}

void FirstStartUp() {
	char code[7] = {0};
	uint8_t index = 0;
	char key;
	
	clearDisplay();

	text_row1("Enter PIN:");

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
				
				for (int i = 0; i < 7; i++) {
					pristupovyPin[i] = code[i];
				}
				
				savePin(code);
				EEPROM_write(FIRST_STARTUP, 0x00);

				text_row1("Pin saved!");
				_delay_ms(1000);

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
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //Povolení pøerušení od pøijmu
	UCSR0C |= (1<<UPM01)|(1<<UCSZ01)|(1<<UCSZ00);
	UBRR0 = 71; //9600

	TCCR1B = (1 << WGM12);                // CTC mód
	TCCR1B |= (1 << CS12) | (1 << CS10);  // Pøeddìlièka 1024
	OCR1A = 108;                          // Hodnota pro 10 ms interval
	TIMSK1 = (1 << OCIE1A);               // Povolení pøerušení na compare match

	DDRA = 0b11111111;
	PORTA= 0b10000000;

	DDRB = 255;
	PORTB = 255;
	
	kb_init();
	i2c_init();
	set_bit(prt(pwr));
	
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
	
	clearDisplay();
	PORTB = 0;
	text_row1("Access allowed!");
	_delay_ms(1000);
	PORTB = 255;
	showMenuText();

}

void AccessDenied(){

	clearDisplay();
	text_row1("Access denied!");
	_delay_ms(1000);
	showMenuText();
}


int findFreeSlot() {
	
	for (int i = 0; i < MAX_CHIPS; i++) {
		
		if (EEPROM_read(STATUS_START_ADDR + i) == 0xFF) { // 255(0xFF) volno, 0(0x00) obsazeno
			return i;
			
		}
	}
	
	return -1; // Když není žádné volné místo
	
}

void saveChip(char chip[16], int slot) {

	EEPROM_write(STATUS_START_ADDR + slot, 0x00);
	
	int baseAddress = DATA_START_ADDR + slot * CHIP_LENGTH;
	
	for (int i = 0; i < CHIP_LENGTH; i++) {
		EEPROM_write(baseAddress + i, chip[i]);
		
	}
	
	
	clearDisplay();
	
	text_row2(chip);
	_delay_ms(500);

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

	return 0; // Všechno v poøádku
	
}

int isChipStored(char chip[16]) {
	
	char buffer[CHIP_LENGTH];

	for (int i = 0; i < MAX_CHIPS; i++) {
		
		if (readChip(i, buffer) == 0 && strcmp(buffer, chip) == 0) {
			return 1; // Èip nalezen
			
		}

	}

	return 0; // Èip nebyl nalezen
	
}

int AddChip() {
	
	startTimer();

	int freeSLot = findFreeSlot();
	
	if(freeSLot == -1){
		return 0; // Pøekroèen maximální poèet èipù

	}

	text_row1("Enclose CHIP");
	text_row2("for ADD");

	while (1) {

		if (data_OK) {

			char chip[16];

			for (int i = 0; i < 16; i++) {
				chip[i] = (char)data_prijata[i + 2];
			}

			data_OK = false;

			if (isChipStored(chip)) {
				
				return 3; // Indikace, že èip existuje
				
			}

			saveChip(chip, freeSLot);
			
			return 2; // Vše ok

		}

		if(timesUP()) {
			return 1; // Èas vypršel
		}
		
		_delay_ms(50);
		
	}
}

int RemoveChip() {

	startTimer();
	
	text_row1("Enclose CHIP");
	text_row2("for REMOVE");

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

					return 0; // Èip byl úspìšnì odstranìn
				}
			}

			return 1; // Èip nebyl nalezen
		}

		if (timesUP()) {
			return 2; // Èas vypršel
			
		}
		_delay_ms(50);
		
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

int verifyUser(){
	
	startTimer();
	
	char code[7] = {0};
	uint8_t index = 0;
	char key;
	
	clearDisplay();

	text_row1("Enter PIN:");

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
				
				if(ComparePin(code)){
					
					text_row1("Verified!");
					_delay_ms(1000);
					
					return 0;
					
					} else {
					
					return 1; // Špatný pin
					
				}
				
				
				
				break;
				} else if (key == '#') {
				
				return 3; // Akce pøerušena
				
			}
		}
		
		if(timesUP()) return 2; // Èas vypršel
		
	}
	
}

void restart() {
	wdt_enable(WDTO_15MS); // Povolit watchdog s timeout 15 ms
	while(1);              // Poèkání dokud watchdog neresetuje MCU
}

int main(void)
{
	
	MCUSR &= ~(1<<WDRF); // Vymazání flagu watchdog resetu
	wdt_disable();       // Vypnutí watchdogu
	
	SetRegistersInit();

	if(EEPROM_read(FIRST_STARTUP) == 0xFF){
		
		FirstStartUp();
		
		} else {
		
		for (int i = 0; i < 6; i++) {
			pristupovyPin[i] = EEPROM_read(PIN_START_ADDR + i);
			
		} pristupovyPin[6] = '\0';
		
	}

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
				
				int resultt = verifyUser();

				if(resultt == 0){
					
					int result = AddChip();
					clearDisplay();
					
					if(result == 0) text_row1("No Space!");
					else if(result == 1) text_row1("Timed out!");
					else if (result == 2) text_row1("Success!");
					else if (result == 3) text_row1("Already Added!");

				} else if (resultt == 1) AccessDenied();
				else if (resultt == 2) text_row1("Timed out!");
				else if (resultt == 3) text_row1("Event cancelled!");

				_delay_ms(1000);
				showMenuText();

				} else if (key == 'B') {
				
				int resultt = verifyUser();
				
				if(resultt == 0){
					
					int result = RemoveChip();
					clearDisplay();

					if (result == 0) text_row1("Chip removed!");
					else if (result == 1) text_row1("Not found!");
					else if (result == 2) text_row1("Timed out!");
					
				} else if (resultt == 1) AccessDenied();
				else if (resultt == 2) text_row1("Timed out!");
				else if (resultt == 3) text_row1("Event cancelled!");

				_delay_ms(1000);
				showMenuText();
				
				} else if (key == 'C') { // Reset PINU
				
				int resultt = verifyUser();
				
				if(resultt == 0){
					clearDisplay();
					EEPROM_write(FIRST_STARTUP, 0xFF);
					text_row1("PIN Removed!");
					restart();
					
				} else if (resultt == 1) AccessDenied();
				else if (resultt == 2) text_row1("Timed out!");
				else if (resultt == 3) text_row1("Event cancelled!");

				_delay_ms(1000);
				showMenuText();
				
			}
		}
	}
}