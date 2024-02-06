#include <stdio.h>
#include <math.h>
#include <stdint.h>

class XYZ1931
{
    float X,Y,Z;
    
    public:
    
    XYZ1931(){
        X=0;
        Y=0;
        Z=0;
    }
    
    XYZ1931(double (*specDistFunc)(uint16_t)){
        double specFuncVal;
        //do a trapezoidal riemann sum to estimate XYZ
        //bounds of integration between 380 and 780 nm wavelengths
        //two bounds of integration should only get added once
        double lowerLim = 380;
        double upperLim = 780;
        uint16_t numBins = 40;
        double binWidth = (upperLim-lowerLim)/numBins;
        //only need one of the first and last values
        specFuncVal = specDistFunc(380);
        X = specFuncVal * Xbar(380) * binWidth/2;
        Y = specFuncVal * Ybar(380) * binWidth/2;
        Z = specFuncVal * Zbar(380) * binWidth/2;
        specFuncVal = specDistFunc(780);
        X = specFuncVal * Xbar(780) * binWidth/2;
        Y = specFuncVal * Ybar(780) * binWidth/2;
        Z = specFuncVal * Zbar(780) * binWidth/2;
        
        for(int i=1; i<numBins; ++i){
            uint16_t wavelen = lowerLim + i * binWidth;
            specFuncVal = specDistFunc(wavelen);
            X = specFuncVal * Xbar(wavelen) * binWidth;
            Y = specFuncVal * Ybar(wavelen) * binWidth;
            Z = specFuncVal * Zbar(wavelen) * binWidth;
        }
        
    }
    
    float Xbar( float wavelen )
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
    float Ybar( float wavelen )
    {
    	float a = 1.014f;
    	float b = 556.3f;
    	float c = 0.075f;
    	float tmp = ( logf( wavelen ) - logf( b ) ) / c;
    	return a*expf(-0.5f*tmp*tmp);
    }
    float Zbar( float wavelen )
    {
    	float a = 1.839f;
    	float b = 449.8f;
    	float c = 0.051f;
    	float tmp = ( logf( wavelen ) - logf( b ) ) / c;
    	return a*expf(-0.5f*tmp*tmp);
    }
};

int main()
{
    printf("wavelen,X,Y,Z\n");
    XYZ1931 test;
    for(int i=380; i<=780; ++i){
        printf("%d,%f,%f,%f\n", i, test.Xbar(i),  test.Ybar(i),  test.Zbar(i));
    }
    return 0;
}
