/*
 * Linear_Motor_Controller.c
 *
 * Created: 26/09/2018 
 * Author : Team 1
 */ 

#include <avr/io.h>
#include "includes.h"

int main(void)
{
    while (1){
		DDRD |= (1<<PD5); 
		PORTD |= (1<<PD5);
    }
}

