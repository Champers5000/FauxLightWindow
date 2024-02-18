/**
 * ALL ANGLES IN RADIANS
 * ALL TIME IN SECONDS FROM START OF DAY
 * ALL DATES IN DAYS FROM START OF YEAR
 * @ref  https://www.pveducation.org/pvcdrom/terrestrial-solar-radiation
*/

#ifndef SunDial_H
#define SunDial_H
#include <stdint.h>

/**
 * @brief Calculates the solar decliniation angle in radians for a given day of the year
 * @param day Day of the year (1-366)
 * @return The solar declination angle in radians
 * @note Formula from  https://solarsena.com/solar-declination-angle-calculator/
*/
float solarDeclination(uint16_t day);

/**
 * @brief Calculates the error (in seconds) from normal time due to eccentricity of the earth's orbit
 * @param day Day of the year (1-366)
 * @return The error (in seconds) from the calculated solar time geographically (through GMT and longitude)
*/
float EoT(uint16_t day);

/**
 * @brief Calculates the local solar time (noon = peak sun of the day, and midnight is opposite of that)
 * @param 
*/
int UTCtoSolarLocal(uint32_t UTCTime, uint16_t day, float longitude);

float hourAngle(uint32_t UTCTime, uint16_t day, float longitude);

float elevationAngle(uint16_t day, uint32_t UTCTime, float latitude, float longitude);

float airMass(float elevationAngle, float elevation);

float airMass(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

float solarIntensity(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

#endif
