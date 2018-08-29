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
#include <avr/eeprom.h>
typedef unsigned char uc;
typedef unsigned short us;

uc period = 25;
uc task_num = 6;

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

uc rabbit[16] = {' ', ' ', ' ', 0, ' ', ' ', ' ', 0,
' ', ' ', ' ', 0, ' ', ' ', ' ', 0};

uc row1_pos, row2_pos, score, ammo;

uc fox[16] = {' ', ' ', ' ', 1, ' ', ' ', ' ', 1,
' ', ' ', ' ', 1, ' ', ' ', ' ', 1};

uc row1[16], row2[16];

uc start = 0;

enum menu_states { menu, press, play, endgame} menu_state = -1;

uc start_button;

int menu_tick(int state) {
	switch(state) {
		case menu:
			start = 0;
			if (start_button) {
				state = press;
			}
			break;
		case press:
			if (!start_button) {
				state = (start) ? menu : play;
			}
			break;
		case play:
			start = 1;
			if (start_button) {
				state = press;
			} else if (ammo == 0) {
				state = endgame;
			}
			break;
		case endgame:
			start = 0;
			if (start_button) {
				state = menu;
				LCD_ClearScreen();
			}
			break;
		default:
			state = menu;
			break;
	}
	
	switch(state) {
		case menu:
			LCD_DisplayString(1, "Press button to begin! HiScore: ");
			LCD_Cursor(32);
			LCD_WriteData(eeprom_read_byte(0) + '0');
			break;
		case press:
			for (uc i = 0; i < 16; i++) {
				row1[i] = rabbit[i];
				row2[i] = fox[i];
			}
			row2_pos = 0;
			row1_pos = 0;
			score = 0;
			ammo = 4;
			break;
		case play:
			break;
		case endgame:
			LCD_DisplayString(1, "Game Over! Your Score is: ");
			LCD_Cursor(27);
			LCD_WriteData(score + '0');
			if (score > eeprom_read_byte(0)) {
				eeprom_write_byte(0, score);
			}
			break;
	}
	
	return state;
}

enum movement_states { m_wait, move} movement_state = -1;

us initial_lr, initial_ud, lr, ud;
uc cursor, row;
uc output;

int movement_tick(int state) {
	switch(state){
		case m_wait:
			if (start) {
				state = move;
			}
			break;
		case move: 
			if (!start) {
				state = m_wait;
				LCD_ClearScreen();
			}
			break;
		default: 
			state = m_wait;
			break;
	}
	switch(state) {
		case m_wait:
			break;
		case move:
			output = 0;
			ADMUX = (ADMUX & 0xF8) | 0x00;
			wait(200);
			lr = ADC;
			ADMUX = (ADMUX & 0xF8) | 0x01;
			wait(200);
			ud = ADC;
			if (row) {
				if (row2[(row2_pos + cursor - 2) % 16] != 1) {
					LCD_Cursor(cursor + (row * 16));
					LCD_WriteData(' ');
				}
			} else {
				if (row1[(row1_pos + cursor - 2) % 16] != 0) {
					LCD_Cursor(cursor + (row * 16));
					LCD_WriteData(' ');
				}
			}
			if (ud < (initial_ud - 150)) {
				//output += 0x01;
				cursor = (cursor > 15) ? 16 : cursor + 1;
			}
			if (ud > (initial_ud + 150)) {
				//output += 0x02;
				cursor = (cursor < 2) ? 1 : cursor - 1;
			}
			if (lr < (initial_lr - 150)) {
				//output += 0x04;
				row = 1;
			}
			if (lr > (initial_lr + 150)) {
				//output += 0x08;
				row = 0;
			}
			LCD_Cursor(cursor + (row * 16));
			LCD_WriteData('+');
			PORTB = output;
			break;
	}
	return state;
}

enum display1_states { d1_wait, display1 } display1_state = -1;

int display1_tick(int state) {
	
	switch(state) {
		case m_wait:
			if (start) {
				state = display1;
			}
			break;
		case display1:
			if (!start) {
				state = d1_wait;
			}
			break;
		default:
			state = d1_wait;
			break;
	}
	
	switch (state) {
		case d1_wait:
			break;
		case display1:
			LCD_Cursor(1);
			for (uc i = 0; i < 16; i++) {
				LCD_WriteData(row1[(row1_pos + i) % 16]);
			}
			row1_pos = (row1_pos < 16) ? row1_pos + 1 : 1;
			break;
			
	}
	
	return state;
}

enum display2_states { d2_wait, display2 } display2_state = -1;

int display2_tick(int state) {
	
	switch(state) {
		case d2_wait:
			if (start) {
				state = display2;
			}
			break;
		case display2:
			if (!start) {
				state = d2_wait;
			}
			break;
		default:
			state = d2_wait;
			break;
	}
	
	switch (state) {
		case d2_wait:
			break;
		case display1:
			LCD_Cursor(17);
			for (uc i = 0; i < 16; i++) {
				LCD_WriteData(row2[(row2_pos + i) % 16]);
			}
			row2_pos = (row2_pos < 16) ? row2_pos + 1 : 1;
			break;
	}
	
	return state;
}

uc bang, loadtime;

enum shoot_states { b_wait, shoot} shoot_state = -1;

void saveScore() {
	eeprom_write_byte(0, score);
}



int shoot_tick(int state) {
	switch(state) {
		case b_wait:
			loadtime = 0;
			if (start && bang) {
				state = shoot;
			}
			break;
		case shoot:
			state = b_wait;
			break;
		default:
			state = b_wait;
			break;
	}
	
	switch(state) {
		case b_wait:
			break;
		case shoot:
			LCD_Cursor(cursor + (row * 16));
			LCD_WriteData(2);
			if (row) {
				if (row2[(row2_pos + cursor - 2) % 16] == 1) {
					score += 2;
				}
				row2[(row2_pos + cursor - 2) % 16] = ' ';
			} else {
				if (row1[(row1_pos + cursor - 2) % 16] == 0) {
					score += 1;
				}
				row1[(row1_pos + cursor - 2) % 16] = ' ';
			}
			ammo--;
			break;
	}
	
	return state;
}

enum ammo_states { count } ammo_state = -1;
	
int ammo_tick(int state) {
	switch(state) {
		case count:
			break;
		default: 
			state = count;
			break;
	}
	
	switch(state) {
		case count:
			switch(ammo) {
				case 0:
					PORTB = 0x00;
					break;
				case 1:
					PORTB = 0x80;
					break;
				case 2:
					PORTB = 0xC0;
					break;
				case 3:
					PORTB = 0xE0;
					break;
				case 4:
					PORTB = 0xF0;
					break;
			}
			break;
	}
	return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
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
	LCD_Rabbit();
	LCD_Bang();
	LCD_Fox();
	
	//LCD_DisplayString(1, "Congratuations");
	
	cursor = 1;
	row = 0;
	
	static task task1, task2, task3, task4, task5, task6;
	
	task1.state = movement_state;
	task1.period = 300;
	task1.elapsedTime = 0;
	task1.TickFct = &movement_tick;
	
	task2.state = display1_state;
	task2.period = 900;
	task2.elapsedTime = 0;
	task2.TickFct = &display1_tick;
	
	task3.state = display2_state;
	task3.period = 600;
	task3.elapsedTime = 0;
	task3.TickFct = &display2_tick;
	
	task4.state = menu_state;
	task4.period = 100;
	task4.elapsedTime = 0;
	task4.TickFct = &menu_tick;
	
	task5.state = shoot_state;
	task5.period = 100;
	task5.elapsedTime = 0;
	task5.TickFct = &shoot_tick;
	
	task6.state = ammo_state;
	task6.period = 100;
	task6.elapsedTime = 0;
	task6.TickFct = &ammo_tick;
	
	task *tasks[] = {&task4, &task2, &task3, &task1, &task5, &task6};
	
	TimerSet(period);
	TimerOn();
	
    /* Replace with your application code */
    while (1) 
    {
		start_button = (~PINA & 0x04) >> 2;
		bang = (~PINA & 0x08) >> 3;
		for (us i = 0; i < task_num; i++ ) {
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

