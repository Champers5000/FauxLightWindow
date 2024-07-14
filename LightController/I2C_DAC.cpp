#include <stdint.h>

#define DAC1
#define DAC2
#define I2C_IS_BUSY()             (internal_status.busy || !(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc))?true:false
//I2C on pins PA2 and PA3

uint8_t calcBaudRate(int F_SCL, float T_RISE){
  return (uint8_t) ((((((float)(F_CPU) / (float)(F_SCL))) - 10 - ((float)(F_CPU) * (T_RISE) / 1000000))) / 2);
}

void DACinit(){
  //enable pullups
  PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
  PORTA.PIN3CTRL |= PORT_PULLUPEN_bm;

  TWI0.CTRLA = 0;
  TWI0.MBAUD = calcBaudRate(100000, 0.1);
  TWI0.MCTRLA = 0x01;
  TWI0.MSTATUS= 0; //disable everything as we setup i2c
  TWI0.MADDR = 0; //host address is 0
  TWI0.MCTRLB = 0;
  TWI0.MDATA = 0;

  TWI0.MCTRLB  |= 0b1000; //set data to be flushed onto the bus as soon as MDATA is written to
  TWI0.MSTATUS |= 0b0001; //set the bus to idle for now
}

bool I2C_Write(uint8_t address, uint8_t *pData, size_t dataLength){
  if(I2C_IS_BUSY()){
    return false;
  }
  TWI0.MADDR = (address << 1) 
  //wait for rx acknowledge and write interrupt flag
  while( !((TWI0.MSTATUS & 0b00010000 == 0) && (TWI0.MSTATUS & 0b01100000 == 0b01100000)) ){
    //when rxack = 0, WIF = 1, and CLKHOLD = 1, we are ready to send more data
  }
  for(i=0; i<dataLength; ++i){
    TWI0.MDATA = pData[i];
    while(TWI0.MSTATUS & 0b00010000){
      if(TWI0.MSTATUS & 0b00001000) //arblost flag
      {
        return false;
      }
    }
  }
  TWI0.MCTRLB |= 0x03; // stop bit

}