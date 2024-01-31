#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "inc/Drivers/StatusLED.h"
#include "inc/Application/FaultState.h"
int main(void) {
  StatusLEDPWMInit();  

  //dummy code to test out pwm of status LED for now
  while (1) {
    for (short i = 0; i <= 0x3FF; ++i) {
      setStatusLEDDuty(i);
      _delay_ms(1);
    }
    for (short i = 0x3FF; i >= 0; --i) {
      setStatusLEDDuty(i);
      _delay_ms(1);
    }
    
    //test fault state
    setStatusLEDDuty(0xFFF);
  }
}


