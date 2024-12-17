#include <math.h>
#define M_PIf 3.141592653589793238462643383279502884F
#include <stdint.h>
#include "inc/Application/SunDial.h"
//reference https://www.pveducation.org/pvcdrom/terrestrial-solar-radiation
SunDial::SunDial(){
    UTCTime=0 ;
    day=0;
    longitude=0;
    latitude=0 ;
    elevation=0;
    airMass=0;
    intensity=0;
}

SunDial::SunDial(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation){
    update(day, UTCTime, latitude, longitude, elevation);
}

uint16_t SunDial::convertDateToDays(uint8_t day, uint8_t month){
  switch(month){
    case 1:
      return day;
    case 2:
      return day +31;
    case 3:
      return day + 31 +28;
    case 4 :
      return day + 31 + 28 + 31;
    case 5:
      return day + 31 + 28 + 31 + 30;
    case 6: 
      return day + 31 + 28 + 31 + 30 + 31;
    case 7: 
      return day + 31 + 28 + 31 + 30 + 31 + 30;
    case 8: 
      return day + 31 + 28 + 31 + 30 + 31 + 30 + 31;
    case 9: 
      return day + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31;
    case 10: 
      return day + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30;
    case 11: 
      return day + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31;
    case 12: 
      return day + 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30;
    default:
      return 0;
  }
}

float SunDial::solarDeclination(uint16_t day){
    //calculate only once a day
    //https://solarsena.com/solar-declination-angle-calculator/
    static uint16_t d=-1;
    static float result;
    if(d==day){
        return result;
    }

    result = asinf(sinf(-23.4397f/180*M_PIf) * cosf(2*M_PIf/365.24f*(day+10)+sinf(2*M_PIf/365.24f*(day-2))/30));
    return result;
}

float SunDial::EoT(uint16_t day){
    //equation of time correction for eccentricity of earth's orbit
    static uint16_t d=-1;
    static float result;
    if(d==day){
        return result;
    }

    result = 592.2f*sinf(4*M_PIf/365.24f*day-8100*M_PIf/9131) - 451.8f*cosf(2*M_PIf/365.24f*day-4050*M_PIf/9131) - 90*sinf(2*M_PIf/365.24f*day-4050*M_PIf/9131);
    return result; //return result in seconds
}

uint32_t SunDial::UTCtoSolarLocal(uint32_t UTCTime, uint16_t day, float longitude){

    // one day is 86400 seconds
    // half a day is 43200 seconds
    int32_t output = (int32_t) (UTCTime + 43200 * longitude / M_PIf + EoT(day));
    if(longitude > 0){
        //we're always gonna be ahead of UTCTime
        return output % 86400; // make sure we wrap around when a new day starts
    }else{
        //we're always behind UTCTime
        if(output < 0){
            return 86400+output;
        }
        return output;
    }
}

float SunDial::hourAngle(uint32_t UTCTime, uint16_t day, float longitude){
    int32_t solarLocal = UTCtoSolarLocal(UTCTime, day, longitude);
    return M_PIf/43200*(solarLocal-43200);
}

float SunDial::elevationAngle(uint16_t day, uint32_t UTCTime, float latitude, float longitude){
    float dAngle = solarDeclination(day);
    float hra = hourAngle(UTCTime, day, longitude);
    return asinf(sinf(dAngle)*sinf(latitude) + cosf(dAngle)*cosf(latitude)*cosf(hra));
}

float SunDial::calcAirMass(float elevationAngle, float elevation){
    //https://en.wikipedia.org/wiki/Air_mass_(solar_energy)
    //effective atmosphere height = 9000m, radius of earth = 6371km
    float zenith = M_PIf/2 - elevationAngle;
    float c = elevation/9000; //ratio of how high we are in the atmosphere
    float r = 6371.0f/9; //ratio of radius of earth/effective atmosphere height
    float airMass = sqrtf(powf((r+c)*cosf(zenith),2)+(r+r+1+c)*(1-c)) - (r+c)*cosf(zenith);
    return airMass;
}

float SunDial::calcAirMass(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation){
    static uint16_t d;
    static uint16_t t;
    static float airmass;
    if(day == d && UTCTime == t){
      return airmass;
    }

    d=day;
    t = UTCTime;
    
    float elevationAng = elevationAngle(day, UTCTime, latitude, longitude);
    airmass = calcAirMass(elevationAng, elevation);
    return airmass;
}

float SunDial::calcSolarIntensity(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation){
    //https://en.wikipedia.org/wiki/Air_mass_(solar_energy)
    float elevationAng = elevationAngle(day, UTCTime, latitude, longitude);
    float aM = calcAirMass(elevationAng, elevation);
    return powf(.7f, powf(aM, .678f))/.7f;
}

float SunDial::planckRadiator(float wavelen, uint16_t temperature){
    wavelen /= 1E9;
    return 1.19024868E-16f/powf(wavelen, 5)/ (expf(.01438776f/wavelen/temperature)-1);
}

float SunDial::sunRadiator(float wavelen){
    //sun color temperature is 5800K
    return planckRadiator(wavelen, 5800);
}

float SunDial::atmosphericRadiator(float wavelen){
    return sunRadiator(wavelen) / expf(airMass * 8.66E-27F/ powf(wavelen/1E9,4));
}

#define PI 3.14159265f
#define H 6.62607015e-34f  // Planck constant
#define C 299792458.0f     // Speed of light
#define K 1.380649e-23f    // Boltzmann constant
float SunDial::atmosphericRadiator1(float wavelen){
  wavelen /= 1e9f;
  float spectral_radiance = (2.0f * H * C * C) / (powf(wavelen, 5.0f)) /
                          (expf((H * C) / (wavelen * K * 5700)) - 1.0f);
  float transmission = expf(-0.1f * airMass * powf(1e-6 / wavelen, 4.0f));
  return spectral_radiance * transmission;
}

void SunDial::update(uint16_t day, uint32_t UTCTime, float latIn, float longIn, float elevation){
    this -> day = day;
    this -> UTCTime = UTCTime;
    this -> latitude = latIn / 180 * M_PIf;
    this -> longitude = longIn / 180 * M_PIf;
    this -> elevation = elevation;
    airMass = calcAirMass(day, UTCTime, this->latitude, this->longitude, elevation);
    intensity=calcSolarIntensity(day, UTCTime, this->latitude, this->longitude, elevation);
}