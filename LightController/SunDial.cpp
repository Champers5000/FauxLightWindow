#include <stdio.h>
#include <math.h>
#include <stdint.h>
//reference https://www.pveducation.org/pvcdrom/terrestrial-solar-radiation

double solarDeclination(uint16_t day){
    //calculate only once a day
    //https://solarsena.com/solar-declination-angle-calculator/
    static uint16_t d=-1;
    static double result;
    if(d==day){
        return result;
    }

    result = asin(sin(-23.4397/180*M_PI) * cos(2*M_PI/365.24*(day+10)+sin(2*M_PI/365.24*(day-2))/30));
    return result;
}

double EoT(uint16_t day){
    //equation of time correction for eccentricity of earth's orbit
    static uint16_t d=-1;
    static double result;
    if(d==day){
        return result;
    }

    result = 592.2*sin(4*M_PI/365.24*day-8100*M_PI/9131) - 451.8*cos(2*M_PI/365.24*day-4050*M_PI/9131) - 90*sin(2*M_PI/365.24*day-4050*M_PI/9131);
    return result; //return result in seconds
}

int UTCtoSolarLocal(uint32_t UTCTime, uint16_t day, double longitude){

    // one day is 86400 seconds
    // half a day is 43200 seconds
    int output = (int) (UTCTime + 43200 * longitude / M_PI + EoT(day));
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

double hourAngle(uint32_t UTCTime, uint16_t day, double longitude){
    int solarLocal = UTCtoSolarLocal(UTCTime, day, longitude);
    return M_PI/43200*(solarLocal-43200);
}

double elevationAngle(uint16_t day, uint32_t UTCTime, double latitude, double longitude){
    double dAngle = solarDeclination(day);
    double hra = hourAngle(UTCTime, day, longitude);
    return asin(sin(dAngle)*sin(latitude) + cos(dAngle)*cos(latitude)*cos(hra));
}

double airMass(double elevationAngle, double elevation){
    //https://en.wikipedia.org/wiki/Air_mass_(solar_energy)
    //effective atmosphere height = 9000m, radius of earth = 6371km
    double zenith = M_PI/2 - elevationAngle;
    double c = elevation/9000; //ratio of how high we are in the atmosphere
    double r = 6371.0/9; //ratio of radius of earth/effective atmosphere height
    double airMass = sqrt(pow((r+c)*cos(zenith),2)+(r+r+1+c)*(1-c)) - (r+c)*cos(zenith);
    return airMass;
    //return 1/(cos(zenith) + .000671302483*pow(1.676911473,-1.6364));
}

double solarIntensity(uint16_t day, uint32_t UTCTime, double latitude, double longitude, double elevation){
    //https://en.wikipedia.org/wiki/Air_mass_(solar_energy)
    double elevationAng = elevationAngle(day, UTCTime, latitude, longitude);
    double aM = airMass(elevationAng, elevation);
    return pow(.7, pow(aM, .678));
}

//reference here for color code
//https://scipython.com/blog/converting-a-spectrum-to-a-colour/

int main()
{
    //austin is 156m above sea level
    //austin coordinates are 30.266666, -97.733330
    for(int i=0; i<86400; i+=300){
        uint32_t timehere = i; //UTCtoSolarLocal(i, 36, -97.733330);
        int hour = timehere / 3600;
        int minute = (timehere % 3600)/60;
        int second = timehere %60;
        printf("Austin's hour angle for %d:%d:%d is %f, intensity= %f\n", hour, minute, second, hourAngle(i, 36, -97.733330), solarIntensity(36, i , 30.266666/180*M_PI, -97.733330/180*M_PI, 156));
    }

    return 0;
}
