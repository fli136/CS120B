/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 2  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned char cnt = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		cnt = 0x00;
		for (unsigned char i = 0; i < 8; i++) {
			if (((PINA >> i) & 0x01) == 0x01) {
				cnt++;
			}
			if (((PINB >> i) & 0x01) == 0x01) {
				cnt++;
			}
		}
		PORTC = cnt;
    }
}

