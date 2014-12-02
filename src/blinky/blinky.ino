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
  Wire.onRequest(handlerFunction);   
  // set for 16 MHz clock, and make sure the LED is off
  //CPU_PRESCALE(CPU_16MHz);
  DDRB |= (1 << 5) | (1 << 6);
  DDRC |= (1 << 5) | (1 << 6);

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
}

void loop(){

}

void handlerFunction()
{
  ADCSRA |= (0 << ADEN);
  ADCSRA |= (1 << ADEN);
  
  ADMUX &= 0;
  ADMUX |= (1 << REFS0) | 0x0;
  // Start the ADC reading.
  ADCSRA |= (1 << ADSC);
  while (!(ADCSRA & (1 << ADIF)));
  
  int value = ADC;
  char A1buf[5];
  char A2buf[5];
  char A3buf[5];
  snprintf(A1buf, sizeof(A1buf), "%d", value);
  serial_write("A1: ", 4);
  serial_write(A1buf, sizeof(A1buf));
  serial_write("\t", 1);
  
  // Reset interrupt flag.
  ADCSRA |= (1 << ADIF);
  
  ADCSRA |= (0 << ADEN);
  ADCSRA |= (1 << ADEN);
  
  ADMUX &= 0;
  ADMUX |= (1 << REFS0) | 0x1;
  // Start the ADC reading.
  ADCSRA |= (1 << ADSC);
  while (!(ADCSRA & (1 << ADIF)));
  
  value = ADC;
  A2buf[5];
  snprintf(A2buf, sizeof(A2buf), "%d", value);
  serial_write("A2: ", 4);
  serial_write(A2buf, sizeof(A2buf));
  serial_write("\t", 1);
  
  // Reset interrupt flag.
  ADCSRA |= (1 << ADIF);
  
  ADCSRA |= (0 << ADEN);
  ADCSRA |= (1 << ADEN);
  
  ADMUX &= 0;
  ADMUX |= (1 << REFS0) | 0x2;
  // Start the ADC reading.
  ADCSRA |= (1 << ADSC);
  while (!(ADCSRA & (1 << ADIF)));
  
  value = ADC;
  A3buf[5];
  snprintf(A3buf, sizeof(A3buf), "%d", value);
  serial_write("A3: ", 4);
  serial_write(A3buf, sizeof(A3buf));
  serial_write("\n", 1);

  char stringToSend[17];
  strcpy(stringToSend, A1buf);
  strcat(stringToSend, " ");  
  strcat(stringToSend, A2buf);  
  strcat(stringToSend, " ");  
  strcat(stringToSend, A3buf);  
  strcat(stringToSend, " ");
  
  Wire.write(stringToSend);  
  // Reset interrupt flag.
  ADCSRA |= (1 << ADIF);
}

void forward(int value)
{
	OCR1A = value;
	OCR1B = 0x3FF;

	OCR3A = 0x3FF;
	OCR3B = value;
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
	OCR1A = right;
	OCR1B = 0x3FF;

	OCR3A = 0x3FF ;
	OCR3B = left;
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
    forward(value);
  }else if(strncmp(command, "bw", 2) == 0)
  {
    backward(value);
  }else if(strncmp(command, "st", 2) == 0)
  {
    serial_write("inside",6);
    forward(1023);
  }
  else if(strncmp(command, "tr", 2) == 0)
  {
   
    turn(rightValue,value);
  }
  
  // print the value to serial
  itoa(value,buffer,10); // 10 = base 10
  
  serial_write(command,2);
  serial_write(buffer,4);
  serial_write("\n",1);
}

void serial_write(char * ch, int charSize)
{
   for(int i =0; i< charSize; i++)
   {
    usb_serial_putchar(ch[i]);
  }
}
