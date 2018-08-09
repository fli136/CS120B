/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 6  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "io.c"
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

unsigned char light;
unsigned char button;
unsigned char score;

enum states { init, light1, light2, light3, press1, press2, release1, release2, release, reset } state = -1;

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

void LT_tick() {
	switch(state) {
		case init: 
			state = light1;
			break;
		case light1:
			state = (button) ? press1 : light2;
			break;
		case light2:
			state = (button) ? press2 : light3;
			break;
		case light3:
			state = (button) ? press1 : light1;
			break;
		case press1:
			state = (button) ? press1 : release1;
			break;
		case press2:
			state = (button) ? press2 : release2;
			break;
		case release1:
			state = release;
			break;
		case release2:
			state = release;
			break;
		case release:
			state = (button) ? reset : release;
			break;
		case reset:
			state = (button) ? reset : light1;
			break;
		default:
			state = init;
			break;
		
	}
	
	switch(state) {
		case init:
			light = 0x00;
			break;
		case light1:
			light = 0x01;
			break;
		case light2:
			light = 0x02;
			break;
		case light3:
			light = 0x04;
			break;
		case press1:
			break;
		case press2:
			break;
		case release1:
			score = (score > 0) ? (score - 1) : score;
			break;
		case release2:
			score = (score < 9) ? (score + 1) : score;
			break;
		case reset:
			break;
		default:
			break;
	
	}
	PORTB = light;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int main()
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	TimerSet(300);
	TimerOn();
	light = 0x00;
	button = 0x00;
	score = 0x00;
	
	LCD_init();
	
	while(1) {
		// User code (i.e. synchSM calls)
		button = ~PINA & 0x01;
		LT_tick();	// Toggle PORTB; Temporary, bad programming style
		
		LCD_ClearScreen();
		
		if (score == 9) {
			LCD_DisplayString(1, "Victory!");
		} else {
			LCD_Cursor(1);
			LCD_WriteData(score + '0');
		}

		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
		// Note: For the above a better style would use a synchSM with TickSM()
		// This example just illustrates the use of the ISR and flag
	}
	return 0;
}