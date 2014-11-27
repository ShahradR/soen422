#include <avr/io.h>
#include <avr/pgmspace.h>

#include <avr/interrupt.h>
#include <string.h> /* memset */
#include <stdio.h>
#include <Wire.h>
//TODO: add to a header file!
//void forward(int);
//void backward(int);
//void hard_left(int);
//void hard_right(int);
//void turn(int, int);
//void callbackfunction(int a);

// Teensy 2.0++
#define F_CPU 16000000
#define CPU_16MHz 0x00
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define SEC 1000
#define PIN_F0 0x0

int value_led_1 = 0;
int value_led_2 = 0;
int value_analog = 0;

int value;

int millisecond_led_1 = 1000;
int millisecond_led_2 = 1000;
int millisecond_analog = 1000;

char serial_value[20];
char serial_buffer[20];

int secondsToCycles(int milliseconds)
{
	return milliseconds * (F_CPU / (2 * 8 * 1023)) / 1000;
}

void setup()
{
  
  int SLAVE_ADDRESS = 0x7;
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(callbackfunction);
  //Serial.begin(38400);
     
	// set for 16 MHz clock, and make sure the LED is off
	//CPU_PRESCALE(CPU_16MHz);
	DDRB |= (1 << 5) | (1 << 6);
	DDRC |= (1 << 5) | (1 << 6);

	// ====== Analog Code ====== 
	// Set analog/digital converter to read on pin F0
	ADMUX |= (1 << REFS0) | PIN_F0;
	
	// Enable ADC
	ADCSRA |= (1 << ADEN);
	// ====================

	// Set the Waveform Generation Mode to Phase Correct PWM, and 
	// when counting up, clear the OCR1A (LED 1) and OCR1B (LED 2)
	// to turn it off, and turn it back on when counting down.
	TCCR1A |= (1 << WGM11) | (1 << WGM10) 
		   | (1 << COM1A1) | (1 << COM1B1);

	TCCR3A |= (1 << WGM31)  | (1 << WGM30)
		   |  (1 << COM3A1) | (1 << COM3B1);

	//Set the prescalar to 8. This is done to make the blinking
	//which controls the brightness to be as fast as possible at
	//its highest brightness.
	TCCR1B |= (1 << CS11);
	TCCR3B |= (1 << CS31);
	
	//Enable timer-based events
	TIMSK1 |= (1 << TOIE1);
	TIMSK3 |= (1 << TOIE3);
	
}

void loop()
{
//	while (1)
//	{
//          //forward(500);
//	}
}

void backward(int value)
{
	OCR1A = value;
	OCR1B = 0x3FF;

	OCR3A = 0x3FF;
	OCR3B = value;
}

void forward(int value)
{
	OCR1A = 0x3FF;
	OCR1B = value;

	OCR3A = value;
	OCR3B = 0x3FF;
}

void hard_right(int value)
{
	OCR1A = 0x3FF;
	OCR1B = value;

	OCR3A = 0x3FF;
	OCR3B = value;
}

void hard_left(int value)
{
	OCR1A = value;
	OCR1B = 0x3FF;

	OCR3A = value;
	OCR3B = 0x3FF;
}

void turn(int right, int left)
{
	OCR1A = 0x3FF;
	OCR1B = left;

	OCR3A = right;
	OCR3B = 0x3FF;
}

void callbackfunction(int a){
  //Serial.println("ijbghibkj");
  char myChar[1000];
  memset(myChar,' ',sizeof(myChar));
  for(int i =0;i< a;i++){
	myChar[i] = Wire.read();
//Serial.println(myChar[i]);

  }
  //Serial.println("yrfhd");

  //if(strncmp(myChar, "hl", 2) == 0)
  //{
  hard_left(500);
  //}
 
}


