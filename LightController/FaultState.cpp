#include "inc/Application/FaultState.h"
#include "inc/Drivers/StatusLED.h"
#include <util/delay.h>

void FaultState(uint8_t type, uint8_t number){
  StatusLEDGPIOInit();
  while(1){
    for(uint8_t i=0; i<type; ++i){
      StatusLEDOn();
      _delay_ms(200);
      StatusLEDOff();
      _delay_ms(200);
    }
    _delay_ms(1000);
    for(uint8_t i=0; i<number; ++i){
      StatusLEDOn();
      _delay_ms(200);
      StatusLEDOff();
      _delay_ms(200);
    }
    _delay_ms(3000);
  }
}
