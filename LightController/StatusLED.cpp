#include "inc/Drivers/StatusLED.h"
#include "inc/Application/FaultState.h"
#include <avr/io.h>
//#include<avr/ioavr32dd20.h>
//home/bob/.arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino7b1/avr/include/avr/ioavr32dd20.h
#define StatusLED (1 << 4)  //status led on pa4
//Timer A WO4
//Also TCD0 WOA
void StatusLEDInit() {
  PORTMUX.TCDROUTEA = 0;  //portmux to tcd0 woa out on pa4
  PORTA.DIRSET |= StatusLED;
  CPU_CCP = CCP_IOREG_gc;
  TCD0.FAULTCTRL |= 0b00010000;  //enable output on WOA on pin PA4

  TCD0.CTRLA |= 0b01100000;  //set clk source to system clock
  TCD0.CTRLB = 0;            //oneramp mode
  TCD0.CTRLC = 0;
  //optional configure reg for functionality
  TCD0.CMPASET = 0;      //turn on when value is 0
  TCD0.CMPACLR = 0;      //turn off when value is 0
  TCD0.CMPBCLR = 0xFFF;  //output compareclearb sets the period
  //optional double buffered registers init here
  /* enable write-protected register */

  while (!(TCD0.STATUS & 0b0001)) {
    //wait until status register says ready to be enabled
  }
  TCD0.CTRLA |= 0b0001;  //enable the timer
}

void StatusLEDOn() {
  TCD0.CMPACLR = 0xFFF;  //max duty cycle
  //sync the double buffered register

  while (!(TCD0.STATUS & 0b0010)) {
    //wait until status register is ready for command
  }
  TCD0.CTRLE |= 0b0010;
}

void StatusLEDOff() {
  TCD0.CMPACLR = 0;  //0 duty cycle
  //sync the double buffered register

  while (!(TCD0.STATUS & 0b0010)) {
    //wait until status register is ready for command
  }
  TCD0.CTRLE |= 0b0010;
}

void StatusLEDToggle() {
  return;  //todo
}

void setStatusLEDDuty(short occValue) {
  if ((occValue >> 12)) {
    // if number has something greater than 12 bits, we come here and throw error
    FaultState(1, 2);
  } else {
    TCD0.CMPACLR = occValue;  //0 duty cycle
    //sync the double buffered register

    while (TCD0.STATUS & 0b0010) {
      //wait until status register is ready for command
    }
    TCD0.CTRLE |= 0b0010;
  }
}
