#include "inc/Drivers/I2C_DAC.h"

MCP4728::MCP4728() : PDbits(0) {
    I2CInit();
    initDAC();
}

void MCP4728::I2CInit() {
    Wire.swap(1);
    Wire.begin();
}

uint16_t MCP4728::brightnessToDac(float brightness)  { 
    return (uint16_t)(brightness * 4090); 
}

void MCP4728::I2CSend(uint8_t* dataArr, uint8_t len) {
    Wire.beginTransmission(I2C_ADDR);
    for(uint8_t i = 0; i < len; ++i) {
        Wire.write(dataArr[i]);
    }
    Wire.endTransmission(I2C_ADDR);
}

void MCP4728::setAllChannels(uint16_t DACVals[4]) {
    for(uint8_t i = 0; i < 4; ++i) {
        uint8_t PDbit = (PDbits & (1 << i)) ? (0b00110000) : 0;
        I2CWriteBuffer[i*2] = PDbit | ((DACVals[i] >> 8) & 0b00001111);
        I2CWriteBuffer[i*2 + 1] = DACVals[i] & 0xFF;
    }
    I2CSend(I2CWriteBuffer, 8);
}

void MCP4728::setAllChannelBrightness(float brightnesses[4]) {
    for(uint8_t i = 0; i < 4; ++i) {
        uint8_t PDbit = (PDbits & (1 << i)) ? (0b00110000) : 0;
        I2CWriteBuffer[i*2] = PDbit | ((brightnessToDac(brightnesses[i]) >> 8) & 0b00001111);
        I2CWriteBuffer[i*2 + 1] = brightnessToDac(brightnesses[i]) & 0xFF;
    }
    I2CSend(I2CWriteBuffer, 8);
}

void MCP4728::setChannel(uint8_t channel, uint16_t DACVal) {
    uint8_t PDbit = (PDbits & (1 << channel)) ? (0b01100000) : 0;
    I2CWriteBuffer[0] = 0b01000000 | (channel << 1);
    I2CWriteBuffer[1] = 0b10010000 | ((DACVal >> 8) & 0x0F) | PDbit;
    I2CWriteBuffer[2] = DACVal & 0xFF;
    I2CSend(I2CWriteBuffer, 3);
}

void MCP4728::writePDbitsToDAC() {
    I2CWriteBuffer[0] = 0b10100000;
    I2CWriteBuffer[1] = 0;
    I2CWriteBuffer[0] |= (PDbits & 0b0001) ? 0b00001100 : 0;
    I2CWriteBuffer[0] |= (PDbits & 0b0010) ? 0b00000011 : 0;
    I2CWriteBuffer[1] |= (PDbits & 0b0100) ? 0b11000000 : 0;
    I2CWriteBuffer[1] |= (PDbits & 0b1000) ? 0b00110000 : 0;
    I2CSend(I2CWriteBuffer, 2);
}

void MCP4728::powerDownChannel(uint8_t chan) {
    PDbits |= 1 << chan;
    writePDbitsToDAC();
}

void MCP4728::powerUpChannel(uint8_t chan) {
    PDbits &= ~(1 << chan);
}

void MCP4728::setPDBits(uint8_t in) {
    PDbits = in;
    writePDbitsToDAC();
}

void MCP4728::initDAC() {
    I2CWriteBuffer[0] = 0b10001111; // set all channels using internal vref
    I2CSend(I2CWriteBuffer, 1);
    setPDBits(0b1000); // not using channel 4
    I2CWriteBuffer[0] = 0b11001111; // set all channels to have a gain of 2
    I2CSend(I2CWriteBuffer, 1);
}

void MCP4728::writeEEPROM(uint16_t val) {
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0b01010000); // EEPROM Write Command for all channels
    for(uint8_t i = 0; i < 4; ++i) {
        Wire.write(((val >> 8) & 0x0F) | 0b10010000);
        Wire.write(val & 0xFF);
    }
    Wire.endTransmission();
}