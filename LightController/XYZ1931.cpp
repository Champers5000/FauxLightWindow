#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#define M_PIf 3.141592653589793238462643383279502884F
#include <stdint.h>
#include "inc/Application/XYZ1931.h"
#include "inc/Application/SunDial.h"

namespace specDistFunc
{
    float planckRadiator(float wavelen, uint16_t temperature){
        wavelen /= 1E9;
        return 1.19024868E-16f/powf(wavelen, 5)/ (expf(.01438776f/wavelen/temperature)-1);
    }

    float sunRadiator(float wavelen){
        //sun color temperature is 5800K
        return planckRadiator(wavelen, 5800);
    }

    float atmosphericRadiator(float wavelen){
        return sunRadiator(wavelen) / expf(airMass * 8.66E-27F/ powf(wavelen/1E9,4));
    }
}

XYZ1931::XYZ1931(){
    X=0;
    Y=0;
    Z=0;
}

XYZ1931::XYZ1931(float (*specDistFunc)(float)){
    float specFuncVal;
    //do a trapezoidal riemann sum to estimate XYZ
    //bounds of integration between 380 and 780 nm wavelengths
    //two bounds of integration should only get added once
    float lowerLim = 380; //units in nanometers
    float upperLim = 780;
    uint16_t numBins = 40;
    float binWidth = (upperLim-lowerLim)/numBins;
    //only need one of the first and last values
    specFuncVal = specDistFunc(lowerLim);
    X = specFuncVal * Xbar(lowerLim) * binWidth/2;
    Y = specFuncVal * Ybar(lowerLim) * binWidth/2;
    Z = specFuncVal * Zbar(lowerLim) * binWidth/2;
    specFuncVal = specDistFunc(upperLim);
    X += specFuncVal * Xbar(upperLim) * binWidth/2;
    Y += specFuncVal * Ybar(upperLim) * binWidth/2;
    Z += specFuncVal * Zbar(upperLim) * binWidth/2;
    for(uint16_t i=1; i<numBins; ++i){
        uint16_t wavelen = lowerLim + i * binWidth;
        specFuncVal = specDistFunc(wavelen);
        X += specFuncVal * Xbar(wavelen) * binWidth;
        Y += specFuncVal * Ybar(wavelen) * binWidth;
        Z += specFuncVal * Zbar(wavelen) * binWidth;
    }
}

float XYZ1931::getCCT(){
    //to calculate CCT, first convert to XY space
    float x = X/(X+Y+Z);
    float y = Y/(X+Y+Z);

    //now use McCamy's approximation
    float n = (x-.3320f)/(y-.1858f);
    return -437*powf(n,3)+3601*powf(n,2)-6861*n+5514.31f;
}

float XYZ1931::Xbar(float wavelen)
{
    float a1 = 1.065f;
    float b1 = 595.8f;
    float c1 = 33.33f;
    float tmp = ( wavelen - b1 ) / c1;
    float bigLobe = a1*expf(-0.5f*tmp*tmp);

    float a2 = 0.3660f;
    float b2 = 446.8f;
    float c2 = 19.44f;
    tmp = ( wavelen - b2 ) / c2;
    float smallLobe = a2*expf(-0.5f*tmp*tmp);

    return smallLobe+bigLobe;
}
float XYZ1931::Ybar( float wavelen )
{
    float a = 1.014f;
    float b = 556.3f;
    float c = 0.075f;
    float tmp = ( logf( wavelen ) - logf( b ) ) / c;
    return a*expf(-0.5f*tmp*tmp);
}
float XYZ1931::Zbar( float wavelen )
{
    float a = 1.839f;
    float b = 449.8f;
    float c = 0.051f;
    float tmp = ( logf( wavelen ) - logf( b ) ) / c;
    return a*expf(-0.5f*tmp*tmp);
}
/*

int main()
{
    printf("wavelen,X,Y,Z\n");

    for(int i=0; i<86400; i+=300){
        uint32_t timehere = UTCtoSolarLocal(i, 36, -97.733330);
        int hour = timehere / 3600;
        int minute = (timehere % 3600)/60;
        int second = timehere %60;
        printf("Austin's hour angle for %d:%d:%d is %f, intensity= %f\n", hour, minute, second, hourAngle(i, 36, -97.733330), solarIntensity(36, i , 30.266666/180*M_PIf, -97.733330/180*M_PIf, 156));
        specDistFunc::airMass = airMass(36, i , 30.266666/180*M_PIf, -97.733330/180*M_PIf, 156);
        XYZ1931 test(&specDistFunc::atmosphericRadiator);
        printf("airmass is %f, cct is %f\n", specDistFunc::airMass, test.getCCT());

    }
    return 0;
}
*/
