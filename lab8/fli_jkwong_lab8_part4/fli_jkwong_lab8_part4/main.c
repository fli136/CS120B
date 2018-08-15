
/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 8 Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}


int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	//DDRD = 0xFF; PORTB = 0x00;
	unsigned char b_char = 0x00;
	//unsigned char d_char = 0x00;
	
	ADC_init();
	
	unsigned short light = ADC;
	unsigned short MAX = 344;
	unsigned short level = MAX >> 3; 
	unsigned char i;
	unsigned char j;
	
    while (1)
    { 
		light = ADC;
		b_char = 0x00;
		j = (char) (light/level);
		for (i = 0; i < j; i++) {
			b_char += (1 << i);
		}
		PORTB = b_char;
    }
	return 0;
}



