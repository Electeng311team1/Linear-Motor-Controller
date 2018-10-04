/*
 * includes.h
 *
 * Created: 26/09/2018 
 *  Author: Team 1 
 */ 
#define F_CPU 8000000

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h> //Remove in final version
#include <stdbool.h>

/*UART Defines*/
//Check if transmission has been received
#define RECEIVE_COMPLETE (UCSR0A & (1 << RXC0))

//Checking to see if the transmission is complete
#define UDR_FULL !(UCSR0A & (1 << UDRE0))

/*UART Functions*/
void uart_initiate(uint16_t UBRR_VALUE);
void uart_transmit(char* message);

/*Message Processing Functions*/
void process_message(char* message, int* mfc);

/*ADC Functions*/

/*HDdriver Functions*/
void driverTimers_Init();
void driverTimersInterrupts_Init();
void setFrequency(float frequency, float dutyCycle);

/*Hdriver volatile variables*/
volatile bool isNegativeCycle;
volatile uint16_t T_OFF1;
volatile uint16_t T_OFF2;
volatile uint16_t T_ON;
