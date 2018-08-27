/*
 * custom.c
 *
 * Created: 8/25/2018 3:28:05 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include <bit.h>
#include <timer.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.c"
#include <string.h>
typedef unsigned char uc;
typedef unsigned short us;

uc period = 25;
uc task_num = 3;

typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

void ADC_init() {
	ADMUX = (1<<REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

void wait(int rounds) {
	unsigned int i;
	for (i = 0; i < rounds; i++) {
		asm("nop");
	}
}

enum movement_states {move} movement_state = -1;

us initial_lr, initial_ud, lr, ud;
uc cursor, row;
uc output;

int movement_tick(int state) {
	switch(state){
		case move: break;
		default: 
			state = move;
			break;
	}
	switch(state) {
		case move:
			output = 0;
			ADMUX = (ADMUX & 0xF8) | 0x00;
			wait(200);
			lr = ADC;
			ADMUX = (ADMUX & 0xF8) | 0x01;
			wait(200);
			ud = ADC;
			if (ud < (initial_ud - 150)) {
				output += 0x01;
				cursor = (cursor > 15) ? 16 : cursor + 1;
			}
			if (ud > (initial_ud + 150)) {
				output += 0x02;
				cursor = (cursor < 2) ? 1 : cursor - 1;
			}
			if (lr < (initial_lr - 150)) {
				output += 0x04;
				row = 1;
			}
			if (lr > (initial_lr + 150)) {
				output += 0x08;
				row = 0;
			}
			LCD_WriteData(' ');
			LCD_Cursor(cursor + (row * 16));
			LCD_WriteData('+');
			PORTB = output;
			break;
	}
	return state;
}

enum display1_states { display1 } display1_state = -1;

uc row1[16] = {' ', ' ', 'A', ' ', ' ', 'A', ' ', ' ', 'A', 
	' ', ' ', 'A', ' ', ' ', 'A', 'E'};

uc row1_pos = 0;

int display1_tick(int state) {
	
	uc i;
	
	switch(state) {
		case display1:
			break;
		default:
			state = display1;
			break;
	}
	
	switch (state) {
		case display1:
			LCD_Cursor(1);
			for (i = 0; i < 16; i++) {
				LCD_WriteData(row1[(row1_pos + i) % 16]);
			}
			row1_pos = (row1_pos < 16) ? row1_pos + 1 : 1;
			break;
			
	}
	
	return state;
}

enum display2_states { display2 } display2_state = -1;

uc row2[16] = {' ', ' ', 'B', ' ', ' ', 'B', ' ', ' ', 'B',
' ', ' ', 'B', ' ', ' ', 'B', 'E'};

uc row2_pos = 0;

int display2_tick(int state) {
	
	uc i;
	
	switch(state) {
		case display2:
		break;
		default:
		state = display2;
		break;
	}
	
	switch (state) {
		case display1:
		LCD_Cursor(17);
		for (i = 0; i < 16; i++) {
			LCD_WriteData(row2[(row2_pos + i) % 16]);
		}
		row2_pos = (row2_pos < 16) ? row2_pos + 1 : 1;
		break;
		
	}
	
	return state;
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	ADC_init();
	ADMUX = (ADMUX & 0xF8) | 0x00;
	wait(1000);
	initial_lr = ADC;
	ADMUX = (ADMUX & 0xF8) | 0x01;
	wait(1000);
	initial_ud = ADC;
	
	LCD_init();
	
	//LCD_DisplayString(1, "Congratuations");
	
	cursor = 1;
	row = 0;
	
	static task task1, task2, task3;
	
	task1.state = movement_state;
	task1.period = 200;
	task1.elapsedTime = 0;
	task1.TickFct = &movement_tick;
	
	task2.state = display1_state;
	task2.period = 400;
	task2.elapsedTime = 0;
	task2.TickFct = &display1_tick;
	
	task3.state = display2_state;
	task3.period = 200;
	task3.elapsedTime = 0;
	task3.TickFct = &display2_tick;
	
	task *tasks[] = {&task2, &task3, &task1};
	
	TimerSet(period);
	TimerOn();
	
    /* Replace with your application code */
	us i;
    while (1) 
    {
		for ( i = 0; i < task_num; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += period;
		}
		while(!TimerFlag);
		TimerFlag = 0;
    }
}

