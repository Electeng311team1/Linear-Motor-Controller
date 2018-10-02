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
#include <stdbool.h> 
#include <util/delay.h>

/*UART Functions*/
void uart_initiate(uint16_t UBRR_VALUE);
void uart_transmit(char* message);

/*ADC Functions*/
void adc_Init();
void adc_SetChannel(uint8_t channel);
double adc_Read(uint8_t channel_sel);
void adc_Calculate();
void getRawData();

/*ADC Volatile variables*/
uint8_t current[SAMPLING_SIZE];
uint8_t currentTime[SAMPLING_SIZE];
uint8_t switchingPointTimes[4];


/*HDdriver Functions*/
void driverTimers_Init();
void driverTimersInterrupts_Init();
void setFrequency(float frequency, float dutyCycle);

/*HDdriver volatile variables*/
volatile bool isNegativeCycle;
volatile uint16_t T_OFF1;
volatile uint16_t T_OFF2;
volatile uint16_t T_ON;
