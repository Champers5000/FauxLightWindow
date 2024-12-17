#ifndef XYZ1931_H
#define XYZ1931_H

#include <stdint.h>

struct xyY
{
    float x;
    float y;
    float Y;
    xyY(float x, float y): x(x), y(y), Y(0){};
    xyY(float x, float y, float Y): x(x), y(y), Y(Y){};
};

class XYZ1931
{
    public:
    float X,Y,Z;
    XYZ1931();
    XYZ1931(float X, float Y, float Z);

    float getCCT();
    static XYZ1931 xyYtoXYZ(xyY in, bool normalize);
    static XYZ1931 XYZFromSpecDistFunc(float (*specDistFunc)(float));

    static xyY XYZtoxyY(XYZ1931 in);
    
    static float Xbar(float wavelen);
    static float Ybar(float wavelen);
    static float Zbar(float wavelen);
};

xyY projectToLine(xyY* target, xyY* point1, xyY* point2);
xyY clipTWP(xyY* target, xyY* illuminantsList, uint8_t numIlluminants);
xyY clipSignal(xyY target, uint8_t numIlluminants);
void calculateWeights(const XYZ1931& target, const XYZ1931 sources[], float weights[], uint8_t numSources);
void calculateWeightsForClippedTarget(xyY target, float* weights, uint8_t numIlluminants);
xyY CCT_To_xyY(float cct);
 #endif