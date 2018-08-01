/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 3  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

enum states { Init, Wait, hWait, IncWait, hRelease, yWait, yRelease, lockPress, lockRelease } state = -1 ;

unsigned char LT_Tick(unsigned char x, unsigned char y, unsigned char h, unsigned char l, unsigned char lock) {
	switch ( state ) { // transitions
		case Init:
			state = Wait;
			break;
		case Wait:
			if ( l && !x && !y && !h ){
				state = lockPress;
			}
			else if ( h && !x && !y && !l){
				state = hWait;
			}
			else{
				state = Wait;
			}
			break;
		case hWait:
			if ( x || y || l ){
				state = IncWait;
			}
			else if ( !y && !x && !h && !l ){
				state = hRelease;
			}
			else {
				state = hWait;
			}
			break;
		case IncWait:
			if ( x || y || h || l ){
				state = IncWait;
			}
			else {
				state = Wait;
			}
			break;
		case hRelease:
			if ( y && !x && !h && !l ){
				state = yWait;
			}
			else if ( h || x || l ){
				state = IncWait;
			}
			else{
				state = hRelease;
			}
			break;
		case yWait:
			if ( x || h || l ){
				state = IncWait;
			}
			else if ( !y && !x && !h && !l ){
				state = yRelease;
			}
			else{
				state = yWait;
			}
			break;
		case yRelease:
			state = Wait;
			break;
		case lockPress:
			if ( x || h || y ){
				state = IncWait;
			}
			else if ( !y && !x && !h && !l ){
				state = lockRelease;
			}
			else {
				state = lockPress;
			}
			break;
		case lockRelease:
			state = Wait;
			break;
		default:
			state = Init;
			break;
	}
	switch ( state ) { // Actions
		case Init:
			lock = 0x00;
			break;
		case Wait:
			break;
		case hWait:
			break;
		case hRelease:
			break;
		case yWait:
			break;
		case yRelease:
			lock = lock ? 0 : 1;
			break;
		case IncWait:
			break;
		case lockPress:
			break;
		case lockRelease:
			lock = 0x00;
			break;
		default:
			break;
	}
	
	PORTC = state;
	PORTB = lock;
	return lock;
}

int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned char x = 0x00;
	unsigned char y = 0x00;
	unsigned char h = 0x00;
	unsigned char l = 0x00;
	unsigned char lock = 0x00;
    while (1) 
    {
		x = PINA & 0x01;
		y = (PINA & 0x02) >> 1;
		h = (PINA & 0x04) >> 2;
		l = (PINA & 0x80) >> 7;
		lock = LT_Tick(x,y,h,l, lock);
		
    }
}

