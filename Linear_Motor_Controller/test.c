/*
 * test.c
 *
 * Created: 1/09/2018 
 *  Author: Team 1
 */ 
#include "includes.h"

void flash_led(){
	DDRB = 0b00100000;
	PORTB = 0b00100000;
	_delay_ms(500);
	PORTB = 0b00000000;
	_delay_ms(500);
}

