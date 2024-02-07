#ifndef SunDial_H
#define SunDial_H

#include <stdint.h>
float solarDeclination(uint16_t day);

float EoT(uint16_t day);

int UTCtoSolarLocal(uint32_t UTCTime, uint16_t day, float longitude);

float hourAngle(uint32_t UTCTime, uint16_t day, float longitude);

float elevationAngle(uint16_t day, uint32_t UTCTime, float latitude, float longitude);

float airMass(float elevationAngle, float elevation);

float airMass(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

float solarIntensity(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

#endif
