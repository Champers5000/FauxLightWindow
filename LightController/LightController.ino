#define F_CPU 24000000UL
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "inc/Drivers/StatusLED.h"
#include "inc/Application/FaultState.h"
#include "inc/Application/XYZ1931.h"

#include <Wire.h>
#include <MCP4725.h>
int8_t DAC0_Initialize(void) {
  // DATA Register
  DAC0.DATA = 0x0;
  // ENABLE enabled; OUTEN enabled; RUNSTDBY enabled;
  DAC0.CTRLA = 0xC1;

  return 0;
}

void DAC0_Enable(void) {
  DAC0.CTRLA |= DAC_ENABLE_bm;
}

void DAC0_Disable(void) {
  DAC0.CTRLA &= ~DAC_ENABLE_bm;
}

void DAC0_SetOutput(uint16_t value) {
  value = value << DAC_DATA_gp;
  value = value & 0xFFC0;
  DAC0.DATA = value;
}

uint8_t DAC0_GetResolution(void) {
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
void TCD0_init(void) {
  /* set the waveform mode */
  TCD0.CTRLB = TCD_WGMODE_DS_gc;
  /* set the signal period */
  TCD0.CMPBCLR = SIGNAL_PERIOD_EXAMPLE_VALUE;
  /* the signals are alternatively active and a small symmetric dead time is
needed */
  TCD0.CMPBSET = SIGNAL_DUTY_CYCLE_EXAMPLE_VALUE + 1;
  TCD0.CMPASET = SIGNAL_DUTY_CYCLE_EXAMPLE_VALUE - 1;
  /* ensure the ENRDY bit is set */
  while (!(TCD0.STATUS & TCD_ENRDY_bm)) {
  }
  TCD0.CTRLA = 0b00000001;
}
void TCD0_enableOutputChannels(void) {
  /* enable write-protected register */
  CPU_CCP = CCP_IOREG_gc;
  TCD0.FAULTCTRL = TCD_CMPAEN_bm    /* enable channel A */
                   | TCD_CMPBEN_bm; /* enable channel B */
}
void PORT_init(void) {
  PORTA.DIR |= PIN4_bm    /* set pin 4 as output */
               | PIN5_bm; /* set pin 5 as output */
}
// int main(void) {
//   PORT_init();
//   TCD0_enableOutputChannels();
//   TCD0_init();
//   /* Replace with your application code */
//   while (1) {
    //this is the TCD test provided by microchip
//   }
// }


MCP4725 MCP(0x60);
void setup(){
  pinMode(PIN_PA4, OUTPUT);
  pinMode(PIN_PD6, OUTPUT);
  //Wire.pins(PIN_PA2,PIN_PA3);
  Wire.swap(0);
  //Wire.usePullups();
  Wire.begin();
  MCP.begin();
  if(MCP.isConnected() == false){
    while(true){
      digitalWrite(PIN_PA4, HIGH);
      delay(200);
      digitalWrite(PIN_PA4,LOW);
      delay(200);
    }
  }
  // int pinlvl = Wire.checkPinLevels();
  // if(pinlvl != 0x03){
  //   while(true){
  //     digitalWrite(PIN_PA4, HIGH);
  //     delay(200);
  //     digitalWrite(PIN_PA4,LOW);
  //     delay(200*pinlvl);
  //   }
  // }
}
void writeDAC(uint16_t reg){
  MCP.setValue(reg);
  return;
  uint8_t addr = (0x60 << 1);
  uint8_t high = (reg >> 4) & 0xFF;
  uint8_t low = ((reg & 0x0F) << 4);
  uint8_t message[3];
  message[0] = addr;
  message[1] = high;
  message[2] = low;
  Wire.beginTransmission(addr);
  Wire.write(0b01000000);
  Wire.write(high);
  Wire.write(low);
  Wire.endTransmission();
}
void loop(){
  for(int i = 0 ; i<=255; ++i){
    analogWrite(PIN_PA4,i);
    analogWrite(PIN_PD6,i);
    writeDAC(i*4);
    delay(10);
  }
  for(int i = 255; i>=0; --i){
    analogWrite(PIN_PA4,i); 
    analogWrite(PIN_PD6,i);
    writeDAC(4*i);
    delay(10);
  }
}
