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
#define StatusLED (1 << 4)  //status led on pa4

// int main(void) {
//   StatusLEDInit();

//   StatusLEDOn();
//   _delay_ms(1000);
//   StatusLEDOff();
//   _delay_ms(1000);
//     StatusLEDOn();
//   _delay_ms(1000);

//   while(1){
//     PORTA.OUT ^=StatusLED; 
//     _delay_ms(200);
//   }

//   // //dummy code to test out pwm of status LED for now
//   // while (1) {
//   //   for (short i = 0; i <= 0x3FF; ++i) {
//   //     setStatusLEDDuty(i);
//   //     _delay_ms(1);
//   //   }
//   //   for (short i = 0x3FF; i >= 0; --i) {
//   //     setStatusLEDDuty(i);
//   //     _delay_ms(1);
//   //   }
    
//   //   //test fault state
//   //   setStatusLEDDuty(0xFFF);
//   // }
// }

#define SIGNAL_PERIOD_EXAMPLE_VALUE (0xC8)
#define SIGNAL_DUTY_CYCLE_EXAMPLE_VALUE (0x64)
/*Using default clock 3.3 MHz */
void TCD0_init(void);
void TCD0_enableOutputChannels(void);
void PORT_init(void);
void TCD0_init(void)
{
/* set the waveform mode */
TCD0.CTRLB = TCD_WGMODE_DS_gc;
/* set the signal period */
TCD0.CMPBCLR = SIGNAL_PERIOD_EXAMPLE_VALUE;
/* the signals are alternatively active and a small symmetric dead time is
needed */
TCD0.CMPBSET = SIGNAL_DUTY_CYCLE_EXAMPLE_VALUE + 1;
TCD0.CMPASET = SIGNAL_DUTY_CYCLE_EXAMPLE_VALUE - 1;
/* ensure the ENRDY bit is set */
while(!(TCD0.STATUS & TCD_ENRDY_bm))
{

}
TCD0.CTRLA = 0b00000001;
}
void TCD0_enableOutputChannels(void)
{
/* enable write-protected register */
CPU_CCP = CCP_IOREG_gc;
TCD0.FAULTCTRL = TCD_CMPAEN_bm /* enable channel A */
| TCD_CMPBEN_bm; /* enable channel B */
}
void PORT_init(void)
{
PORTA.DIR |= PIN4_bm /* set pin 4 as output */
| PIN5_bm; /* set pin 5 as output */
}
int main(void)
{
PORT_init();
TCD0_enableOutputChannels();
TCD0_init();
/* Replace with your application code */
while (1)
{
;
}
}
