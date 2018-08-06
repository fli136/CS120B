/*
 * fli_jkwong_lab4_part3.c
 *
 * Created: 8/6/2018 12:19:27 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

enum states {init, light1p, light1r, light2p, light2r, light3p, light3r} state =-1;
unsigned char button;
unsigned char sequence;

void LT_tick() {
	switch(state) {//transitions
		case init:
			state = light1r;
			break;
		case light1r:
			state = (button) ? light2p : light1r;
			break;
		case light1p:
			state = (button) ? light1p : light2r;
			break;
		case light2r:
			state = (button) ? light3p : light2r;
			break;
		case light2p:
			state = (button) ? light2p : light3r;
			break;
		case light3r:
			state = (button) ? light1p : light3r;
			break;
		case light3p:
			state = (button) ? light3p : light1r;
			break;
		default:
			state = init;
			break;
	}
	
	switch(state) {//transitions
		case init:
			break;
		case light1r:
			sequence = 0x15;
			break;
		case light1p:
			break;
		case light2r:
			sequence = 0x2A;
			break;
		case light2p:
			break;
		case light3r:
			sequence = 0x33;
			break;
		case light3p:
			break;
	}
	
	PORTB = sequence & 0x37;
} 

int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	button = 0x00;
	sequence = 0x00;
	
    while (1) 
    {
		button = 0x01 & ~PINA;
		LT_tick();
    }
}

