#define F_CPU 8000000UL
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "inc/Drivers/StatusLED.h"
#include "inc/Application/FaultState.h"
#include "inc/Application/XYZ1931.h"

int8_t DAC0_Initialize(void)
{
    // DATA Register 
	DAC0.DATA = 0x0;
    // ENABLE enabled; OUTEN enabled; RUNSTDBY enabled; 
	DAC0.CTRLA = 0xC1;

    return 0;
}

void DAC0_Enable(void)
{
    DAC0.CTRLA |= DAC_ENABLE_bm;
}

void DAC0_Disable(void)
{
    DAC0.CTRLA &= ~DAC_ENABLE_bm;
}

void DAC0_SetOutput(uint16_t value)
{
    value     = value << DAC_DATA_gp;
	value     = value & 0xFFC0;
    DAC0.DATA = value;
}

uint8_t DAC0_GetResolution(void)
{
    return 10;
}

int main(void) {
  StatusLEDInit();  

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


