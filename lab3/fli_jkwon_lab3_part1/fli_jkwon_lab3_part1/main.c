/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 3  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

enum States { Init, Press1, LED2, Press2, LED1 } state;

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
    /* Replace with your application code */
	unsigned char A0 = 0x00;
	state = Init;
	
    while (1) 
    {
		A0 = PINA & 0x01;
		switch(state) {
			case Init:
				PORTB = 0x01;
				if (A0) {
					state = Press1;
				}
				break;
				
			case Press1:
				if (!A0) {
					state = LED2;
				}
				break;
			
			case LED2:
				PORTB = 0x02;
				if (A0) {
					state = Press2;
				}
				break;
				
			case Press2:
				if (!A0) {
					state = LED1;
				}
				break;
				
			case LED1:
				PORTB = 0x01;
				if (A0) {
					state = Press1;
				}
				break;
			
			default:
				state = Init;
				break;
		}
    }
}

