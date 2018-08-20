/*
 * fli_jkwong_lab9_part3.c
 *
 * Created: 8/16/2018 3:13:02 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define E5 659.25
#define C5 523.25
#define G4 392.00
#define E4 329.63
#define F5 698.46
#define A4 440.00
#define C4 261.63
#define A4b 415.30
#define B4b 466.16
#define F4 349.23
#define E4 329.63
#define D4 293.66

double scale[48] = {659.25, 0, 659.25, 0, 0, 0, 659.25, 0, 0, 0, 523.25, 0, 659.25, 0, 0, 0,
					 783.99, 783.99, 783.99, 0, 0, 0, 0, 0, 392.00, 392.00, 392.00, 0, 0, 0, 0, 0,
					 C5, C5, C5, 0, 0, 0, G4, G4, 0, 0, 0, 0, E4, E4, 0, 0};
					 
double scale2[144] = {C5, 0, C5, 0, C5, 0, C5, C5, C5, C5, C5, 0, A4b, A4b, A4b, A4b, A4b, 0, B4b,
					B4b, B4b, B4b, B4b, 0, C5, 0, 0, 0, B4b, 0, C5, C5, C5, C5, C5, C5, C5, C5,
					 C5, C5, C5, C5, C5, C5, C5, C5, C5, 0,
					 G4, G4, G4, G4, G4, 0, F4, F4, F4, F4, F4, 0, G4, G4, G4, G4, G4, 0,
					 F4, F4, 0, B4b, B4b, B4b, B4b, B4b, 0, B4b, B4b, 0, A4, A4, A4, A4, A4, 0,
					 B4b, B4b, 0, A4, A4, A4, A4, A4, 0, A4, A4, 0, G4, G4, G4, G4, G4, 0,
					 F4, F4, F4, F4, F4, 0, E4, E4, E4, E4, E4, 0, F4, F4, 0, D4, D4, D4,
					 D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, D4, 
					  D4, D4, D4, D4, D4, 0}; 

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

unsigned char cnt;

void playing() {
	set_PWM(scale2[cnt]);
	cnt++;
}

enum states { init, wait, press, play, release} state = -1;
unsigned char A0;
 
void tick() {
	switch(state) {
		case init:
			state = wait;
			break;
		case wait:
			if (A0) {
				state = press;
			}
			break;
		case press:
			state = play;
			break;
		case play:
			if (cnt > 144) {
				state = release;
			}
			break;
		case release:
			if (!A0) {
				state = wait;
			}
			cnt = 0;
			break;
		default:
			state = init;
			break;
	}
	switch(state) {
		case init:
			break;
		case wait:
			break;
		case press:
			break;
		case play:
			playing();
			break;
		case release:
			set_PWM(0);
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	TimerSet(100);
	TimerOn();
	
	PWM_on();
	cnt = 0;
	set_PWM(0);
    /* Replace with your application code */
    while (1) 
    {
		A0 = ~PINA & 0x01;
		tick();
		while (!TimerFlag);
		TimerFlag = 0;
    }
}

