/*
 * fli034_lab1_part4.c
 *
 * Created: 7/30/2018 3:06:04 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs
	DDRC = 0x00; PORTC = 0xFF; // Configure port C's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs
    /* Replace with your application code */
	unsigned short tmp1 = 0;
	short tmp2 = 0;
	unsigned char total = 0x00;
    while (1) 
    {
		tmp1 = PINA + PINB + PINC;
		tmp2 = PINA - PINC;
		total = (PINA >> 2) + (PINB >> 2) + (PINC >> 2) >> 2 << 2;
		if (tmp2 > 80 || tmp2 < -80) {
			total = (total & 0xFC) | 0x02;
		}
		
		if (tmp1 > 140) {
			total = (total & 0xFE) | 0x01;
		}
		
		PORTD = total;
    }
	
	return 0;
}

