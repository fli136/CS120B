/*
 * fli034_lab2_part5.c
 *
 * Created: 7/31/2018 2:05:53 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}

int main(void)
{
	DDRB = 0x06; PORTB = 0xFF;
	DDRD = 0x00; PORTD = 0xFF;
	
	unsigned short weight = 0;
	unsigned char lights = 0;
    /* Replace with your application code */
    while (1) 
    {
		lights = 0x00;
		weight = (PIND << 1) + (PINB & 0x01);
		if (weight >= 70) {
			lights = SetBit(lights, 1, 1);
		} else if (weight > 5) {
			lights = SetBit(lights, 2, 1);
		}
		
		PORTB = (lights & 0x06) | (PINB & 0x01);
		
    }
}

