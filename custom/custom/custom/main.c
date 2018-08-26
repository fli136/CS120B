/*
 * custom.c
 *
 * Created: 8/25/2018 3:28:05 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include "io.c"
typedef unsigned char uc;
typedef unsigned short us;

void ADC_init() {
	ADMUX = (1<<REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

void wait(int rounds) {
	unsigned int i;
	for (i = 0; i < rounds; i++) {
		asm("nop");
	}
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	ADC_init();
	uc cursor, row;
	uc output = 0;
	us initial_lr, initial_ud, lr, ud;
	ADMUX = (ADMUX & 0xF8) | 0x00;
	wait(1000);
	initial_lr = ADC;
	ADMUX = (ADMUX & 0xF8) | 0x01;
	wait(1000);
	initial_ud = ADC;
	
	LCD_init();
	
	//LCD_DisplayString(1, "Congratuations");
	
	cursor = 1;
	row = 0;
	
    /* Replace with your application code */
    while (1) 
    {
		output = 0;
		ADMUX = (ADMUX & 0xF8) | 0x00;
		wait(200);
		lr = ADC;
		ADMUX = (ADMUX & 0xF8) | 0x01;
		wait(200);
		ud = ADC;
		if (lr < (initial_lr - 30)) {
			output += 0x01;
			cursor = (cursor < 2) ? 1 : cursor - 1;
		}
		if (lr > (initial_lr + 30)) {
			output += 0x02;
			cursor = (cursor > 15) ? 16 : cursor + 1;
		}
		if (ud < (initial_ud - 30)) {
			output += 0x04;
			row = 1;
		}
		if (ud > (initial_ud + 30)) {
			output += 0x08;
			row = 0;
		}
		LCD_Cursor(cursor + (row * 16));
		PORTB = output;
    }
}

