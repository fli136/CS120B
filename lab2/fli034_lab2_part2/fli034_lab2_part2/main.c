/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 2  Exercise 2 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	
	unsigned char lights = 0x00;
	
    /* Replace with your application code */
    while (1) 
    {
		lights = 0x00;
		if (PINA > 1) {
			lights = SetBit(lights, 5, 1);
		}
		if (PINA > 3) {
			lights = SetBit(lights, 4, 1);
		}
		if (PINA > 5) {
			lights = SetBit(lights, 3, 1);
		}
		if (PINA > 7) {
			lights = SetBit(lights, 2, 1);
		}
		if (PINA > 10) {
			lights = SetBit(lights, 1, 1);
		}
		if (PINA > 1) {
			lights = SetBit(lights, 0, 1);
		}
		if (PINA < 5) {
			lights = SetBit(lights, 6, 1);
		}
		
		PORTC = lights;
    }
}

