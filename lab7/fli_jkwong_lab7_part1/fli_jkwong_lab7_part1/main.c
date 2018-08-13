/*
 * fli_jkwong_lab7_part1.c
 *
 * Created: 8/13/2018 12:08:09 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

enum three_States { three_init, three1, three2, three3, three4 } threeStates = -1;
enum blink_states { blink_init, blinkOn, blinkOff } blinkStates = -1;
enum combine_states { combine_init, set } combine_states = -1;
unsigned char three_out;
unsigned char blink_out;

void three_Tick() {
	switch (threeStates) {
		case three_init:
			threeStates = three1;
			break;
		case three1:
			threeStates = three2;
			break;
		case three2:
			threeStates = three3;
			break;
		case three3:
			threeStates = three4;
			break;
		case three4:
			threeStates = three1;
			break;
		default:
			threeStates = three_init;
			break;
	}
	switch (threeStates) {
		case three_init:
			break;
		case three1:
			three_out = 0x01;
			break;
		case three2:
			three_out = 0x02;
			break;
		case three3:
			three_out = 0x04;
			break;
		case three4:
			three_out = 0x08;
			break;
	}
}

void blink_Tick() {
	switch (blinkStates) {
		case blink_init:
			blinkStates = blinkOn;
			break;
		case blinkOn:
			blinkStates = blinkOff;
			break;
		case blinkOff:
			blinkStates = blinkOn;
			break;
		default:
			blinkStates = blink_init;
			break;
	}
	
	switch (blinkStates) {
		case blink_init:
			break;
		case blinkOn:
			blink_out = 0x08;
			break;
		case blinkOff:
			blink_out = 0x00;
			break;
	}
}

void combine_Tick() {
	switch (combine_states) {
		case combine_init:
			combine_states = set;
			break;
		case set:
			break;	
		default:
			combine_states = combine_init;
			break;
	}
	
	switch (combine_states) {
		case combine_init:
			break;
		case set:
			PORTB = three_out | blink_out;
			break;
	}
}


volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

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
    /* Replace with your application code */
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	three_out = 0x00;
	blink_out = 0x00;
    while (1) 
    {
		three_Tick();
		blink_Tick();
		combine_Tick();
		while(!TimerFlag);
		TimerFlag = 0;
    }
	return 0;
}

