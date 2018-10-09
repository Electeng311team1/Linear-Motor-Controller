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

/*UART Defines*/
//Check if transmission has been received
#define RECEIVE_COMPLETE (UCSR0A & (1 << RXC0))

//Checking to see if the transmission is complete
#define UDR_FULL !(UCSR0A & (1 << UDRE0))

/*Motor Driver Function*/
void driver_timer_initiate(void);
void set_parameters(float frequency,  uint8_t mfc);
void soft_start(float req_freq, int req_mfc);

/*Motor Driver Global Variables*/
volatile bool isNegative;
volatile bool change_duty;
volatile bool first_cycle;
volatile uint16_t t1_compare_a;
volatile uint16_t t1_compare_b;


/*ADC Functions*/
void adc_initiate();
void adc_start(float* operating_frequency);

/*ADC defines*/
#define SAMPLING_SIZE 150

/*ADC variables*/
volatile uint8_t compA_count;

/*Other Functions*/
//#define SITH
void project_skywalker();

/*Other Variables*/
volatile uint16_t testadcvalue;
volatile bool testvalue;

/*ADC Functions*/
// void adc_Init();
// void adc_SetChannel(uint8_t channel);
// double adc_Read(uint8_t channel_sel);

float calculateAverageSupply();
float calculateCurrentRMS();
float calculatePower(float Vsupply_average );

void process_message(char* message, int* mfc);

//float c_scaler = ;
#define v_scaler 2.6
#define Vref 5.0

/*ADC Volatile variables*/
#define SAMPLING_SIZE 150




uint16_t current[SAMPLING_SIZE];
uint16_t currentTime[SAMPLING_SIZE];

float Pinst[SAMPLING_SIZE];

volatile uint16_t switchingPointTimes[4];
volatile uint16_t voltage[15];
volatile uint16_t voltageTime[15];
volatile uint8_t channel_sel;
volatile uint8_t isCalculating;
volatile uint8_t ovf_count; 
volatile uint8_t beginCalculation;
volatile uint8_t i;
volatile uint8_t j;

/*ADC Functions*/
void adc_initiate();
void adc_start(float* operating_frequency);

/*ADC defines*/
#define SAMPLING_SIZE 150

/*ADC variables*/
volatile uint8_t compA_count;

/*Other Functions*/
//#define SITH
void project_skywalker();

/*Other Variables*/
volatile uint16_t testadcvalue;
volatile bool testvalue;

/*HDdriver Functions*/
void driverTimers_Init();
void driverTimersInterrupts_Init();
void setFrequency(float frequency, float dutyCycle);

/*HDdriver volatile variables*/
volatile bool isNegativeCycle;
volatile uint16_t T_OFF1;
volatile uint16_t T_OFF2;
volatile uint16_t T_ON;
volatile float OPERATING_FREQUENCY;
volatile uint8_t stopDriver;

float calculateResonantFrequency();