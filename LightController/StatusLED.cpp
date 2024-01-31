#include "inc/Drivers/StatusLED.h"
#include "inc/Application/FaultState.h"
#include <avr/io.h>

#define StatusLED (1 << 4)//status led on pb4

void StatusLEDGPIOInit(){
    TCCR1A &= 0b11001111; //disable OC in order to give led control back to gpio
    DDRB |= StatusLED;
}

void StatusLEDOn(){
  PORTB |= StatusLED;
}

void StatusLEDOff(){
  PORTB &= ~StatusLED;
}

void StatusLEDToggle(){
  PORTB ^= StatusLED;
}

void StatusLEDPWMInit(){
  DDRB |= StatusLED;
  TCCR1A = 0b00100011;  // phase correct PWM mode (10bit), clearing oc1B on compare match
  TCCR1B = 0b00000001;  //no prescaler clock
}

void setStatusLEDDuty(short occValue) {
  ///0x03FF is the max value
  if (!(occValue >> 10)){
    OCR1B = occValue;
  }
  else
  {
    // if number has something greater than 10 bits, we come here and throw error
    FaultState(1,2);
  }
}
