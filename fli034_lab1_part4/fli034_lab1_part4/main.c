/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 1  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as inputs
	DDRC = 0x00; PORTC = 0xFF; // Configure port C's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs
    /* Replace with your application code */
	unsigned short weight = 0;
	signed short balance  = 0;
	unsigned char total = 0x00;
    while (1) 
    {
		weight = PINA + PINB + PINC;
		balance = PINA - PINC;
		total = (PINA >> 2) + (PINB >> 2) + (PINC >> 2);
		if (balance > 80 || balance < -80) {
			total = (total & 0xFC) | 0x02;
		}
		
		if (weight > 140) {
			total = (total & 0xFE) | 0x01;
		}
		
		PORTD = total;
    }
	
	return 0;
}

