#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//status led on pb4
void FaultState(){

}

void setStatusLEDDuty(short occValue) {
  ///0x03FF is the max value
  if (!(occValue >> 10)){
    OCR1B = occValue;
  }
  else
  {
    // if number has something greater than 10 bits, we come here
    // TO DO: throw into error state
    FaultState();
  }
}


int main(void) {
  DDRB |= (1 << 4);  // LED on PB4, configure bit 4 as output in data direction register
  //led on PB6 = OC1B
  TCCR1A = 0b00100011;  // phase correct PWM mode (10bit), clearing oc1B on compare match
  TCCR1B = 0b00000001;  //no prescaler clock
  //this sets the counter TOP value to 10 bit pwm (0x03FF)
  setStatusLEDDuty(0x10);

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
  }
}
