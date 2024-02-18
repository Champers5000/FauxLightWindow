#ifndef FaultState_H
#define FaultState_H

#include <stdint.h>

/**
 * @brief Call this function from anywhere when the code detects an error. The status LED will blink out the error type and number 
 * @param type The number of blinks the LED will flash first
 * @param number The number of blinks the LED will flash second
 * @note Once faultstate is entered, the MCU must be reset 
*/
void FaultState(uint8_t type, uint8_t number);

#endif
