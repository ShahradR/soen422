#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h> /* memset */
#include <stdio.h>
#include <Wire.h>
extern "C"{
#include "usb_serial.h"
};


#define SEC 1000
#define PIN_F0 0x0


char buffer[9];
void setup()
{
  
  usb_init();

  int SLAVE_ADDRESS = 0x7;
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(callbackfunction);
       
  // set for 16 MHz clock, and make sure the LED is off
  //CPU_PRESCALE(CPU_16MHz);
  DDRB |= (1 << 5) | (1 << 6);
  DDRC |= (1 << 5) | (1 << 6);

  // ====== Analog Code ====== 
  // Set analog/digital converter to read on pin F0
//  ADMUX |= (1 << REFS0) | PIN_F0;
	
  // Enable ADC
//  ADCSRA |= (1 << ADEN);
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
  //TIMSK1 |= (1 << TOIE1);
  //TIMSK3 |= (1 << TOIE3);
  
  // backward(0);	
}

void loop()
{
  // Do Nothing in Loop.
}

void forward(int value)
{
  serial_write("inside forward \n",15);
	OCR1A = value;
	OCR1B = 0x3FF;

	OCR3A = 0x3FF;
	OCR3B = value;
 serial_write("end forward \n",12);
 
}

void backward(int value)
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

void callbackfunction(int numChars) {
  
  // declare variables
  char myChar[1000];
  short space = 1;
  char command[2];
  char charValue[4];
  int value =0;
  int leftValue =0;
  int rightValue = 0;
  // end 
  
  // read the value from begalebone
  memset(myChar,' ',sizeof(myChar));
  for(int i =0; i< numChars; i++){
    myChar[i] = Wire.read();
    usb_serial_putchar(myChar[i]);
  }
   serial_write("\n",1);
  
  // get the command
  for (int i = 0; i < sizeof(command); i++)
  {
    command[i] = myChar[i];
  }
// 
// // get the value
 int counter = 0;
  for (int i=sizeof(command) + space; ;i++ )
  {
     if(myChar[i] == '~')
     {
       value=  atoi(charValue);
       break;
     }else if (myChar[i] == ','){
       
       rightValue=  atoi(charValue);
       memset(charValue,0,sizeof(charValue));
       counter = 0;
       i++;
     }
     
     charValue[counter++] = myChar[i];  
  }
  
  
  
  
  if(strncmp(command, "hl", 2) == 0)
  {
    hard_left(value);
  } else if(strncmp(command, "hr", 2) == 0)
  {
    hard_right(value);
  }else if(strncmp(command, "fw", 2) == 0)
  {
   // forward(value);
  }else if(strncmp(command, "bw", 2) == 0)
  {
    //backward(value);
  }else if(strncmp(command, "st", 2) == 0)
  {
    serial_write("inside",6);
    forward(1023);
  }
  else if(strncmp(command, "tr", 2) == 0)
  {
   
   // turn(rightValue,value);
  }
  
  
  
  
  
  
  
  
  // print the value to serial
  itoa(value,buffer,10); // 10 = base 10
  
  serial_write(command,2);
  serial_write(buffer,4);
  serial_write("\n",1);
}





void serial_write(char * ch, int charSize)
{
   for(int i =0; i< charSize; i++){
    usb_serial_putchar(ch[i]);
  }
}


