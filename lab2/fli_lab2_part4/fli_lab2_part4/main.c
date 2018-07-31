/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 2  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>





int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		PORTB = (PINA & 0xF0) >> 4;
		PORTC = (PINA & 0x0F) << 4;
    }
}

