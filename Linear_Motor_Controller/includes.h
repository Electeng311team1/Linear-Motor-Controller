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
void adc_Begin();
void adc_SetChannel(uint8_t channel);
double adc_Read(uint8_t channel_sel);

//float c_scaler = ;


/*ADC Volatile variables*/
#define SAMPLING_SIZE 150




uint16_t current[SAMPLING_SIZE];
uint16_t currentTime[SAMPLING_SIZE];

volatile uint16_t switchingPointTimes[4];
volatile uint16_t voltage[15];
volatile uint16_t voltageTime[15];
volatile uint8_t channel_sel;
volatile uint8_t isCalculating;
volatile uint8_t ovf_count; 




/*HDdriver Functions*/
void driverTimers_Init();
void driverTimersInterrupts_Init();
void setFrequency(float frequency, float dutyCycle);

#define SET_SW1 PORTB |= (1<<PB0)
#define SET_SW2 PORTD |= (1<<PD5)
#define SET_SW3 PORTD |= (1<<PD7)
#define SET_SW4 PORTD |= (1<<PD6)

#define CLEAR_SW1 PORTB &= ~(1<<PB0)
#define CLEAR_SW2 PORTD &= ~(1<<PD5)
#define CLEAR_SW3 PORTD &= ~(1<<PD7)
#define CLEAR_SW4 PORTD &= ~(1<<PD6)

/*HDdriver volatile variables*/
volatile bool isNegativeCycle;
volatile uint16_t T_OFF1;
volatile uint16_t T_OFF2;
volatile uint16_t T_ON;
volatile float OPERATING_FREQUENCY;
volatile uint8_t stopDriver;

/*Calculation Function*/
float calculateAverageSupply();
float calculateCurrentRMS();
float calculatePower(float Vsupply_average);
float calculateResonantFrequency();

/*Calculation Variables*/

 uint16_t switchTime1;
 uint16_t switchTime2;
 uint16_t switchTime3;
 uint16_t switchTime4;
 
volatile  uint8_t calculateResonant;
#define RESONANTDECAYTIME 0.15
uint8_t resonantsamplingcomplete;

/*HalfDrive variables/functions*/
uint8_t setHalfDrive;
#define LOWDUTYTHRESHOLD 0.3 //can be changed after testing
