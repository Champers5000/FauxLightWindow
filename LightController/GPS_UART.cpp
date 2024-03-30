#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//tx on pd4, rx on pd5
void UARTInit(){
  PORTMUX.USARTROUTEA = 0x03;//portmux
  USART0.BAUD = 10000;//set usart baud rate to 9600, assume peripheral clk is 24MHz
  USART0.CTRLC |= 0b00000011;//frame format mode of operation, assume 8 bit data frames
  PORTD.DIRSET |= (1<<4); //txd pin set as output pin
  USART0.CTRLB |= 0b11000000;//enable transmit and receive
  USART0.CTRLA |= 0b10000000;//enable interrupts
}

void GPSInit(){
  UARTInit();
}
