#ifndef StatusLEDDriver
#define StatusLEDDriver
void StatusLEDInit();

void StatusLEDOn();

void StatusLEDOff();

void StatusLEDToggle();

//implementation of PWM for status StatusLED
void StatusLEDPWMInit();

void setStatusLEDDuty(short occValue);
#endif
