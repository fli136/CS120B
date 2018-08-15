/*
 * GccApplication1.c
 *
 * Created: 8/15/2018 12:13:35 PM
 * Author : ucrcse
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
	DDRD = 0xFF; PORTB = 0x00;
	unsigned char b_char = 0x00;
	unsigned char d_char = 0x00;
	
	ADC_init();
	
	unsigned short x = ADC;
	
    while (1)
    { 
		x = ADC;
		b_char = (char) x;
		d_char = (char) (x >> 6);
		PORTB = b_char;
		PORTD = d_char & 0x03;
    }
	return 0;
}

