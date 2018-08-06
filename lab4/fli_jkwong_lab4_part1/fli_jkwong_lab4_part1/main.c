/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 4  Exercise 1 
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
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned char lights = 0x00;
	unsigned char fuel;
	
    /* Replace with your application code */
    while (1) 
    {
		lights = 0x00;
		fuel = 0x0F & ~PINA;
		if (fuel > 0) {
			lights = SetBit(lights, 5, 1);
		}
		if (fuel > 2) {
			lights = SetBit(lights, 4, 1);
		}
		if (fuel > 4) {
			lights = SetBit(lights, 3, 1);
		}
		if (fuel > 6) {
			lights = SetBit(lights, 2, 1);
		}
		if (fuel > 9) {
			lights = SetBit(lights, 1, 1);
		}
		if (fuel > 12) {
			lights = SetBit(lights, 0, 1);
		}
		if (fuel < 5) {
			lights = SetBit(lights, 6, 1);
		}
		
		PORTB = lights;
    }
}
