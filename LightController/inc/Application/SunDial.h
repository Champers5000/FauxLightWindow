/**
 * ALL ANGLES IN RADIANS
 * ALL TIME IN SECONDS FROM START OF DAY
 * ALL DATES IN DAYS FROM START OF YEAR
 * @ref  https://www.pveducation.org/pvcdrom/terrestrial-solar-radiation
*/

#ifndef SunDial_H
#define SunDial_H
#include <stdint.h>

class SunDial{
    public:
    uint32_t UTCTime;
    uint16_t day;
    float longitude;
    float latitude;
    float elevation;
    float airMass;
    float intensity;

SunDial();

SunDial(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

static uint16_t convertDateToDays(uint8_t day, uint8_t month);

/**
 * @brief Calculates the solar decliniation angle in radians for a given day of the year
 * @param day Day of the year (1-366)
 * @return The solar declination angle in radians
 * @note Formula from  https://solarsena.com/solar-declination-angle-calculator/
*/
static float solarDeclination(uint16_t day);

/**
 * @brief Calculates the error (in seconds) from normal time due to eccentricity of the earth's orbit
 * @param day Day of the year (1-366)
 * @return The error (in seconds) from the calculated solar time geographically (through GMT and longitude)
*/
static float EoT(uint16_t day);

/**
 * @brief Calculates the local solar time (noon = peak sun of the day, and midnight is opposite of that)
 * @param 
*/
static uint32_t UTCtoSolarLocal(uint32_t UTCTime, uint16_t day, float longitude);

static float hourAngle(uint32_t UTCTime, uint16_t day, float longitude);

static float elevationAngle(uint16_t day, uint32_t UTCTime, float latitude, float longitude);

static float calcAirMass(float elevationAngle, float elevation);

static float calcAirMass(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

static float calcSolarIntensity(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

void update(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation);

static float planckRadiator(float wavelen, uint16_t temperature);

static float sunRadiator(float wavelen);

float atmosphericRadiator(float wavelen);
float atmosphericRadiator1(float wavelen);

};
#endif