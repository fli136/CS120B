/*	Partner(s) Name & E-mail: Jasmine Kwong jkwon045@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab 6 Exercise 1
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

enum states { Init, Wait, dPress, iPress, bPress } state = -1;
unsigned char time_cnt;
unsigned char cnt;
unsigned char A0;
unsigned char A1;

void LT_TickFct() {
	
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
			if (!A1 && A0) {
				state = iPress;
				time_cnt = 0;
			} else if (A1 && A0) {
				state = bPress;
				time_cnt = 0;
			} else if (!A1 && !A0) {
				state = Wait;
			}
			time_cnt++;
			
			break;
		case iPress:
			if (!A0 && A1) {
				state = dPress;
				time_cnt = 0;
			} else if (A1 && A0) {
				state = bPress;
				time_cnt = 0;
			} else if (!A1 && !A0) {
				state = Wait;
			}
			time_cnt++;
			break;
		case bPress:
			if (!A0 && !A1) {
				state = Wait;
			} else if (!A0 && A1) {
				state = dPress;
			} else if (!A1 && A0) {
				state = iPress;
			}
			break;
		default:
			state = Init;
			break;
	}
	
	switch(state) {
		case Init:
			cnt = 0;
			break;
		case Wait:
			time_cnt = 0;
			break;
		case dPress:
			if (((time_cnt + 9) % 10) > 8) {
				cnt = (cnt > 0) ? (cnt - 1) : cnt;
			}
			break;
		case iPress:
			if (((time_cnt + 9) % 10) > 8) {
				cnt = (cnt < 9) ? (cnt + 1) : cnt;
			}
			break;
		case bPress:
			cnt = 0;
			break;
		default:
			break;
	}

}

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


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	TimerSet(100);
	TimerOn();
	cnt = 0;
	time_cnt = 0;
	
	
	// Initializes the LCD display
	LCD_init();
	
	// Starting at position 1 on the LCD screen, writes Hello World
	LCD_Cursor(1);
	
	
	while(1) {
		A0 = ~PINA & 0x01;
		A1 = ~PINA & 0x02;
		LT_TickFct();
		LCD_ClearScreen();
		LCD_Cursor(1);
		LCD_WriteData(cnt + '0');
		while(!TimerFlag);
		TimerFlag = 0;
	}
}


