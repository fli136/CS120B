/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 3  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

enum states { Init, Wait, dPress, dRelease, iPress, iRelease, bPress, bRelease } state = -1;

unsigned char LT_TickFct(unsigned char cnt, unsigned char A0, unsigned char A1) {
		
	switch(state) {
		case Init:
			state = Wait;
			break;
		case Wait:
			if (A0 && A1) {
				state = bPress;
				} else if (A0) {
				state = iPress;
				} else if (A1) {
				state = dPress;
			}
			break;
		case dPress:
			if (!A1) {
				state = dRelease;
			} else if (A0) {
				state = bPress;
			}
			break;
		case dRelease:
			state = Wait;
			break;
		case iPress:
			if (!A0) {
				state = iRelease;
			} else if (A1) {
				state = bPress;
			}
			break;
		case iRelease:
			state = Wait;
			break;
		case bPress:
			if (!A0 && !A1) {
				state = bRelease;
			}
			break;
		case bRelease:
			state = Wait;
			break;
		default:
			state = Init;
			break;
	}
	
	switch(state) {
		case Init:
			cnt = 7;
			break;
		case Wait:
			break;
		case dPress:
			break;
		case dRelease:
			cnt = (cnt > 0) ? (cnt - 1) : cnt;
			break;
		case iPress:
			break;
		case iRelease:
			cnt = (cnt < 9) ? (cnt + 1) : cnt;
			break;
		case bPress:
			break;
		case bRelease:
			cnt = 0;
			break;
		default:
			cnt = 7;
			break;
	}
	PORTC = cnt;
	return cnt;
}

int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned char cnt = 0x00;
	unsigned char A0 = 0x00;
	unsigned char A1 = 0x00;
	
    while (1) 
    {
		A0 = PINA & 0x01;
		A1 = (PINA & 0x02) >> 1;
		cnt = LT_TickFct(cnt, A0, A1);
    }
}

