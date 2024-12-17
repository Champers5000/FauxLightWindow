//#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#define M_PIf 3.141592653589793238462643383279502884F
#include <stdint.h>
#include "inc/Application/XYZ1931.h"
// #include "inc/Application/SunDial.h"

xyY myIlluminants[4] =
{
    xyY(0.51992,0.40386), //2000K
    xyY(0.43693,0.40407), //approximated 3000K white
    xyY(0.28265,0.2429),  //TV cold white
    xyY(0,0)    
}; //put in order of warmest to coldest temperature
const xyY whitePoint = xyY(.31272,.32903);

uint8_t numSeries[4] = {6,5,6,0};
uint8_t  minSeries = 5;

XYZ1931::XYZ1931(){
    X=0;
    Y=0;
    Z=0;
}

XYZ1931::XYZ1931(float X, float Y, float Z) {
    this -> X = X;
    this -> Y = Y;
    this -> Z = Z;
}

XYZ1931 XYZ1931::XYZFromSpecDistFunc(float (*specDistFunc)(float)) {
    float X,Y,Z;
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
    X /= Y;
    Z/=Y;
    Y=1;

    return XYZ1931(X,Y,Z);
}

float XYZ1931::getCCT(){
    //to calculate CCT, first convert to XY space
    float x = X/(X+Y+Z);
    float y = Y/(X+Y+Z);
    
    //now use McCamy's approximation
    float n = (x-.3320f)/(y-.1858f);
    return -437*powf(n,3)+3601*powf(n,2)-6861*n+5514.31f;
}


XYZ1931 XYZ1931::xyYtoXYZ(xyY in, bool normalize){
    //input in format xyY, must be array of length 3
    XYZ1931 out = XYZ1931();
    out.Y = normalize ? 1: in.Y;
    float normalizedY =  out.Y / in.y;

    out.X = in.x * normalizedY;
    out.Z = (1-in.x - in.y) * normalizedY;
    return out;
}

xyY XYZ1931::XYZtoxyY(XYZ1931 in){

    float sum = in.X + in.Y + in.Z;
    
    return xyY(in.X/sum, in.Y/sum, in.Y);
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


bool pointInTriangle(xyY* target,xyY* illuminantsList, uint8_t numIlluminants)
{
    float as_x = target -> x -illuminantsList[0].x;
    float as_y = target -> y -illuminantsList[0].y;

    bool s_ab = (illuminantsList[1].x -illuminantsList[0].x) * as_y - (illuminantsList[1].y -illuminantsList[0].y) * as_x > 0;

    if ((illuminantsList[2].x -illuminantsList[0].x) * as_y - (illuminantsList[2].y -illuminantsList[0].y) * as_x > 0 == s_ab) 
        return false;
    if ((illuminantsList[2].x - illuminantsList[1].x) * (target -> y - illuminantsList[1].y) - (illuminantsList[2].y - illuminantsList[1].y)*(target -> x - illuminantsList[1].x) > 0 != s_ab) 
        return false;
    return true;
}

xyY projectToLine(xyY* target, xyY* point1, xyY* point2) {
    //put point1 at the origin to make linear algebra work
    xyY p2(point2 -> x - point1 -> x, point2 -> y - point1 -> y);
    //p2 is now the vector point from point1 to point2
    xyY newTarget (target -> x - point1 -> x, target -> y - point1 -> y);
    //now we need to project the newTarget onto the line
    float scalingFactor = (p2.x * newTarget.x + p2.y * newTarget.y) / (p2.x * p2.x + p2.y * p2.y);
    if(scalingFactor < 0){
        scalingFactor = 0;
    }
    if(scalingFactor > 1){
        scalingFactor = 1;
    }
    newTarget.x = p2.x * scalingFactor + point1 -> x;
    newTarget.y = p2.y * scalingFactor + point1 -> y;
    return newTarget;
}

xyY clipTWP(xyY* target, xyY* illuminantsList, uint8_t numIlluminants){
    // //check each line segment of the triangle for insection with the line to the whitepoint
    // for(uint8_t i=0; i < numIlluminants; ++i){
    //     uint8_t index2 = i+1;
    //     if(i+1>= numIlluminants){
    //         index2 = 0;
    //     }

    //     //center the whitepoint at the origin
    //     xyY newTarget = xyY(target -> x - whitePoint.x, target -> y - whitePoint.y);
    //     float slope = newTarget.y / newTarget.x;
    //     uint8_t quadrantcode = ((newTarget.y < 0) << 1)| (newTarget.x < 0 );

    //     float slope2 = (illuminantsList[index2].y - illuminantsList[i].y) / (illuminantsList[index2].x - illuminantsList[i].x);
    //     float b2 = (illuminantsList[i].y - whitePoint.y) - (illuminantsList[i].x - whitePoint.y)* slope2;
    //     xyY intersection(b2/(slope-slope2), 0);
    //     intersection.y = slope * intersection.x + whitePoint.y;
    //     intersection.x += whitePoint.x;
    //     float maxX;
    //     float minX;
    //     float maxY;
    //     float minY;
    //     if(illuminantsList[i].x >illuminantsList[index2].x){
    //         maxX = illuminantsList[i].x;
    //         minX = illuminantsList[index2].x;
    //     }else{
    //         minX = illuminantsList[i].x;
    //         maxX = illuminantsList[index2].x;
    //     }
    //     if(illuminantsList[i].y >illuminantsList[index2].y){
    //         maxY = illuminantsList[i].y;
    //         minY = illuminantsList[index2].y;
    //     }else{
    //         minY = illuminantsList[i].y;
    //         maxY = illuminantsList[index2].y;
    //     }

    //     if(intersection.x <= maxX && intersection.x >= minX && intersection.y <= maxY && intersection.y >= minY){
    //         return intersection;
    //     }
    // }
    //if we reach this point, we failed to project towards white point, try doing project to nearest point inside the gamut
    xyY output = xyY(0,0);
    float minDistance = 2.0f;
    for(uint8_t i=0; i < numIlluminants; ++i){
        uint8_t index2 = i+1;
        if(i+1>= numIlluminants){
            index2 = 0;
        }

        xyY projected = projectToLine(target, &illuminantsList[i], &illuminantsList[index2]);
        float dx = (target -> x - projected.x) ;
        float dy = (target -> y - projected.y) ;
        float distance = dx*dx + dy*dy;
        if(distance < minDistance){
            minDistance = distance;
            output.x = projected.x;
            output.y = projected.y;
        }
    }
    return output;
}

xyY clipSignal(xyY target, uint8_t numIlluminants){
    switch(numIlluminants){
        case 2:
            return projectToLine(&target, &myIlluminants[0], &myIlluminants[1]);
        case 3:
            if(pointInTriangle(&target, myIlluminants, numIlluminants)){
                return target;
            }
            return clipTWP(&target, myIlluminants, numIlluminants);
        default:
            return xyY(0,0);
    }

}

void calculateWeights(const XYZ1931& target, const XYZ1931 sources[], float weights[], uint8_t numSources) {
    // // Construct the coefficient matrix A
    // float a[3][4];  // Augmented matrix

    // // Copy sources and target into augmented matrix
    // for (int i = 0; i < 3; i++) {
    //     a[i][0] = sources[i].X;
    //     a[i][1] = sources[i].Y;
    //     a[i][2] = sources[i].Z;
    // }
    // a[0][3] = target.X;
    // a[1][3] = target.Y;
    // a[2][3] = target.Z;

    // // Forward elimination
    // for (int i = 0; i < 3; i++) {
    //     // Find pivot
    //     int max_row = i;
    //     for (int k = i + 1; k < 3; k++) {
    //         if (fabsf(a[k][i]) > fabsf(a[max_row][i])) {
    //             max_row = k;
    //         }
    //     }

    //     // Swap maximum row with current row
    //     for (int j = i; j < 4; j++) {
    //         float temp = a[max_row][j];
    //         a[max_row][j] = a[i][j];
    //         a[i][j] = temp;
    //     }

    //     // Make all rows below this one 0 in current column
    //     for (int k = i + 1; k < 3; k++) {
    //         float c = -a[k][i] / a[i][i];
    //         for (int j = i; j < 4; j++) {
    //             if (i == j) {
    //                 a[k][j] = 0;
    //             } else {
    //                 a[k][j] += c * a[i][j];
    //             }
    //         }
    //     }
    // }

    // // Back substitution
    // weights[2] = a[2][3] / a[2][2];
    // weights[1] = (a[1][3] - a[1][2] * weights[2]) / a[1][1];
    // weights[0] = (a[0][3] - a[0][2] * weights[2] - a[0][1] * weights[1]) / a[0][0];


    const float learningRate = 0.01f;
    const uint32_t maxIterations = 1000;
    const float epsilon = 0.0001f;

    // Initialize weights equally
    for (uint8_t i = 0; i < numSources; ++i) {
        weights[i] = 1.0f / numSources;
    }

    for (uint32_t iteration = 0; iteration < maxIterations; ++iteration) {
        XYZ1931 combinedXYZ;
        
        // Calculate current combined XYZ
        for (uint8_t i = 0; i < numSources; ++i) {
            combinedXYZ.X += sources[i].X * weights[i];
            combinedXYZ.Y += sources[i].Y * weights[i];
            combinedXYZ.Z += sources[i].Z * weights[i];
        }

        // Calculate error
        float errorX = target.X - combinedXYZ.X;
        float errorY = target.Y - combinedXYZ.Y;
        float errorZ = target.Z - combinedXYZ.Z;

        // Check if error is small enough
        if (fabs(errorX) < epsilon && fabs(errorY) < epsilon && fabs(errorZ) < epsilon) {
            break;
        }

        // Update weights
        for (uint8_t i = 0; i < numSources; ++i) {
            weights[i] += learningRate * (
                errorX * sources[i].X +
                errorY * sources[i].Y +
                errorZ * sources[i].Z
            );

            // Ensure weight is non-negative
            if (weights[i] < 0) weights[i] = 0;
        }

        // Normalize weights
        float sum = 0;
        for (uint8_t i = 0; i < numSources; ++i) {
            sum += weights[i];
        }
        for (uint8_t i = 0; i < numSources; ++i) {
            weights[i] /= sum;
        }
    }
}

void calculateWeightsForClippedTarget(xyY target, float* weights, uint8_t numIlluminants){
    //first check if target is within our color space, if not then run the clipping algorithm
    xyY clippedTarget = clipSignal(target, numIlluminants);

    XYZ1931 xyzTarget = XYZ1931::xyYtoXYZ(clippedTarget, true);

    XYZ1931 sourcesToLinearlyCombine[numIlluminants];
    for(uint8_t i = 0; i < numIlluminants; ++i){
        sourcesToLinearlyCombine[i] = XYZ1931::xyYtoXYZ(myIlluminants[i], true);
    }

    //calculate the weights for each illuminant
    calculateWeights(xyzTarget, sourcesToLinearlyCombine, weights, numIlluminants);

    //scale the weights based on number of leds in series
    for(uint8_t i=0; i<numIlluminants; ++i){
        weights[i] *= (float)minSeries / numSeries[i];
    }
    
    return; 
}

xyY CCT_To_xyY(float cct) {
    //https://en.wikipedia.org/wiki/Planckian_locus#Approximation
    float x, y;

    // Ensure CCT is within valid range
    if (cct < 1000.0f) cct = 1000.0f;
    if (cct > 15000.0f) cct = 15000.0f;

    // Calculate inverse CCT
    float inv_cct = 1.0f / cct;

    // Calculate x
    if (cct <= 4000.0f) {
        x = -0.2661239f * 1e9f * powf(inv_cct, 3) 
          - 0.2343589f * 1e6f * powf(inv_cct, 2) 
          + 0.8776956f * 1e3f * inv_cct 
          + 0.179910f;
    } else {
        x = -3.0258469f * powf(10, 9) * powf(inv_cct, 3) 
          + 2.1070379f * powf(10, 6) * powf(inv_cct, 2) 
          + 0.2226347f * powf(10, 3) * inv_cct 
          + 0.240390f;
    }

    // Calculate y
    if (cct < 2222.0f) {
        y = -1.1063814f * powf(x, 3) 
          - 1.34811020f * powf(x, 2) 
          + 2.18555832f * x 
          - 0.20219683f;
    } else if(cct < 4000.0f){
        y = -0.9549476f * powf(x, 3) 
          - 1.37418593f * powf(x, 2) 
          + 2.09137015f * x 
          - 0.16748867f;
    } else{
        y = 3.08758f * powf(x, 3) 
          - 5.87733867f * powf(x, 2) 
          + 3.75112997f * x 
          - 0.37001483f;
    }

    return xyY(x,y);
}