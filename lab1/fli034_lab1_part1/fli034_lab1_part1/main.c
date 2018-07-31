/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 1  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpC = 0x00; // Temporary variable to hold the value of C
	unsigned char tmpB = 0x00; 	// Temporary variable to hold the value of B
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	while(1)
	{
		// (1) Read input
		tmpA = PINA & 0x01;
		tmpB = PINA & 0x02;
		// (2) Perform computation
		// if PA0 is 1, set PB1PB0=01, else=10
		if (tmpA == 0x01 && tmpB == 0x00) { // True if PA0 is 1
			tmpC = (tmpC & 0xFE) | 0x01; // Sets tmpC to 01
		}
		else {
			tmpC = (tmpC & 0xFE) | 0x00; // Sets tmpB to bbbbbb10
			// (clear rightmost 2 bits, then set to 10)
		}
		// (3) Write output
		PORTB = tmpC;
	}
	return 0;
}

