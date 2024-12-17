#ifndef MCP4728_H
#define MCP4728_H

#include <stdint.h>
#include <Wire.h>

class MCP4728 {
public:

    uint8_t PDbits;
    uint16_t DACVal[4];
    uint8_t I2CWriteBuffer[30];
    static const uint8_t I2C_ADDR = 0b1100000;;

    static uint16_t brightnessToDac(float Y);
    void I2CInit();
    void I2CSend(uint8_t* dataArr, uint8_t len);
    void writePDbitsToDAC();
    void initDAC();

    MCP4728();

    void setAllChannels(uint16_t DACVals[4]);
    void setAllChannelBrightness(float DACVals[4]);
    void setChannel(uint8_t channel, uint16_t DACVal);
    void powerDownChannel(uint8_t chan);
    void powerUpChannel(uint8_t chan);
    void setPDBits(uint8_t in);
    void writeEEPROM();
};

#endif // MCP4728_H