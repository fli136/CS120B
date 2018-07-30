/*
 * fli034_lab1_part3.c
 *
 * Created: 7/30/2018 2:44:25 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char spot0 = 0x00;
	unsigned char spot1 = 0x00;
	unsigned char spot2 = 0x00;
	unsigned char spot3 = 0x00;
	unsigned char cntavail = 0x00;
	//unsigned char temp = 0x00; //
	while(1)
	{
		// (1) Read input
		spot0 = PINA & 0x01;
		spot1 = PINA & 0x02;
		spot2 = PINA & 0x04;
		spot3 = PINA & 0x08;
		cntavail = 0x00;
		// (2) Perform computation

		if (spot0 == 0x00) {
			cntavail++;
		}
		if (spot1 == 0x00) {
			cntavail++;
		}
		if (spot2 == 0x00) {
			cntavail++;
		}
		if (spot3 == 0x00) {
			cntavail++;
		}
		
		if (cntavail == 0x0F) {
			cntavail = cntavail | 0x8F;
		}
		
		// (3) Write output
		PORTC = cntavail;
	}
	return 0;
}

