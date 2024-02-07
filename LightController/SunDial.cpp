#include <stdio.h>
#include <math.h>
#define M_PIf 3.141592653589793238462643383279502884F
#include <stdint.h>
#include "inc/Application/SunDial.h"
//reference https://www.pveducation.org/pvcdrom/terrestrial-solar-radiation

float solarDeclination(uint16_t day){
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

float EoT(uint16_t day){
    //equation of time correction for eccentricity of earth's orbit
    static uint16_t d=-1;
    static float result;
    if(d==day){
        return result;
    }

    result = 592.2f*sinf(4*M_PIf/365.24f*day-8100*M_PIf/9131) - 451.8f*cosf(2*M_PIf/365.24f*day-4050*M_PIf/9131) - 90*sinf(2*M_PIf/365.24f*day-4050*M_PIf/9131);
    return result; //return result in seconds
}

int UTCtoSolarLocal(uint32_t UTCTime, uint16_t day, float longitude){

    // one day is 86400 seconds
    // half a day is 43200 seconds
    int output = (int) (UTCTime + 43200 * longitude / M_PIf + EoT(day));
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

float hourAngle(uint32_t UTCTime, uint16_t day, float longitude){
    int solarLocal = UTCtoSolarLocal(UTCTime, day, longitude);
    return M_PIf/43200*(solarLocal-43200);
}

float elevationAngle(uint16_t day, uint32_t UTCTime, float latitude, float longitude){
    float dAngle = solarDeclination(day);
    float hra = hourAngle(UTCTime, day, longitude);
    return asinf(sinf(dAngle)*sinf(latitude) + cosf(dAngle)*cosf(latitude)*cosf(hra));
}

float airMass(float elevationAngle, float elevation){
    //https://en.wikipedia.org/wiki/Air_mass_(solar_energy)
    //effective atmosphere height = 9000m, radius of earth = 6371km
    float zenith = M_PIf/2 - elevationAngle;
    float c = elevation/9000; //ratio of how high we are in the atmosphere
    float r = 6371.0f/9; //ratio of radius of earth/effective atmosphere height
    float airMass = sqrtf(powf((r+c)*cosf(zenith),2)+(r+r+1+c)*(1-c)) - (r+c)*cosf(zenith);
    return airMass;
}

float airMass(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation){
    float elevationAng = elevationAngle(day, UTCTime, latitude, longitude);
    return airMass(elevationAng, elevation);
}

float solarIntensity(uint16_t day, uint32_t UTCTime, float latitude, float longitude, float elevation){
    //https://en.wikipedia.org/wiki/Air_mass_(solar_energy)
    float elevationAng = elevationAngle(day, UTCTime, latitude, longitude);
    float aM = airMass(elevationAng, elevation);
    return powf(.7f, powf(aM, .678f));
}

//reference here for color code
//https://scipython.com/blog/converting-a-spectrum-to-a-colour/
/*
int main()
{
    //austin is 156m above sea level
    //austin coordinates are 30.266666, -97.733330
    for(int i=0; i<86400; i+=300){
        uint32_t timehere = i; //UTCtoSolarLocal(i, 36, -97.733330);
        int hour = timehere / 3600;
        int minute = (timehere % 3600)/60;
        int second = timehere %60;
        printf("Austin's hour angle for %d:%d:%d is %f, intensity= %f\n", hour, minute, second, hourAngle(i, 36, -97.733330), solarIntensity(36, i , 30.266666/180*M_PIf, -97.733330/180*M_PIf, 156));
    }

    return 0;
}
*/
