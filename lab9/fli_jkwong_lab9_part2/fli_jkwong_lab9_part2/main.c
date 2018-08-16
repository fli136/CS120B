/*
 * fli_jkwong_lab9_part2.c
 *
 * Created: 8/16/2018 2:40:19 PM
 * Author : ucrcse
 */ 


#include <avr/io.h>

double scale[8] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM0A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

unsigned char A0, A1, A2, tone;

enum states {init, wait, upP, upR, dP, dR } state = -1;
	
void scale_Tick() {
	switch(state) {
		case init:
			state = wait;
			break;
		case wait:
			if (A0 && !A1) {
				state = upP;
			} else if (!A0 && A1) {
				state = dP;
			}
			break;
		case upP:
			state = (A0) ? upP : upR;
			break;
		case dP:
			state = (A1) ? dP: dR;
			break;
		case upR:
			state = wait;
			break;
		case dR:
			state = wait;
			break;
		default:
			state = init;
			break;
	}
	
	switch(state) {
		case init:
			tone = 0;
			break;
		case wait:
			break;
		case upP:
			break;
		case dP:
			break;
		case upR:
			tone = (tone < 7) ? tone + 1 : tone;
			break;
		case dR:
			tone = (tone > 0) ? tone - 1 : tone;
			break;
	}		
}
	
enum playing { off, on } play = -1;

void play_Tick() {
	switch (play) {
		case off:
			if (A2) {
				play = on;
			}
			break;
		case on:
			if (A2) {
				play = off;
				
			}
			break;
		default:
			play = off;
			break;	
	}
	
	switch (play) {
		case off:
			set_PWM(0);
			break;
		case on:
			set_PWM(scale[tone]);
			break;
	}
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PWM_on();
    /* Replace with your application code */
    while (1) 
    {
		A0 = ~PINA & 0x01;
		A1 = (~PINA & 0x02) >> 1;
		A2 = (~PINA & 0x04) >> 2;
		scale_Tick();
		play_Tick();
    }
	return 0;
}

