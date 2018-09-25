/*
 * includes.h
 *
 * Created: 1/09/2018
 *  Author: Team 1
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <math.h>
#include <string.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>

/*Communication Function*/
void initialise_uart(uint16_t UBRR_VALUE);
void uart_transmit(char character);
void transmit_message(char* message);
char* format_message();
void uart_flush();
unsigned char uart_receive();
char* receive_message();
void append(char* s, unsigned char character);

/*Test Functions*/
void flash_led();
