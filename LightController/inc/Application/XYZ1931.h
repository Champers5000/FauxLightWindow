#ifndef XYZ1931_H
#define XYZ1931_H

#include <stdint.h>

namespace specDistFunc{
    float planckRadiator(float wavelen, uint16_t temperature);

    float sunRadiator(float wavelen);

    static float airMass = 1;
    float atmosphericRadiator(float wavelen);
}

class XYZ1931
{
    public:
    float X,Y,Z;
    XYZ1931();

    XYZ1931(float (*specDistFunc)(float));

    float getCCT();

    static float Xbar(float wavelen);
    static float Ybar( float wavelen );
    static float Zbar( float wavelen );
};

#endif
