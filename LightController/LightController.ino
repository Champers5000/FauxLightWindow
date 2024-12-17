#define F_CPU 24000000UL
#include <stdint.h>

#include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>


// #include "inc/Drivers/StatusLED.h"
#include "inc/Application/FaultState.h"
#include "inc/Application/XYZ1931.h"
#include "inc/Application/SunDial.h"
#include "inc/Drivers/I2C_DAC.h"

#include <Wire.h>
#include <TinyGPSPlus.h>

#define NUM_ILLUMINANTS 3

TinyGPSPlus gps;
SunDial sd;
MCP4728 dac;
char buf[100];
float specDistFromSunDial(float wavelen) {
  return sd.atmosphericRadiator(wavelen);
}

void setLed(bool a, bool b) {
  digitalWrite(PIN_PC1, a);
  digitalWrite(PIN_PC2, b);
}
//very basic color calculator to run on HW for now
void setup() {
  // PORTA.DIRSET = 0xF0; // PA7 through PA4 represent channel 3-0 enable bits
  // PORTA.OUT = 0;


  Serial.swap(3);
  Serial1.swap(2);

  Serial.begin(9600);
  Serial1.begin(115200);

  PORTC.DIRSET = 0b0110;
  PORTC.OUT = 0;
  delay(2000);

  // DAC Init
  dac = MCP4728();
  // Finished DAC Init

  // uint16_t allChannels[4] = {1000,2000,3000,4000};
  // while(true){
  //   setLed(0,0);
  //   dac.setAllChannels(allChannels);

  //   // delay(10000);
  //   // setLed(0,1);
  //   // dac.setPDBits(0b1100);
  //   // delay(10000);
  //   // setLed(1,0);
  //   // dac.setChannel(0,500);
  //   // dac.setChannel(1,1500);
  //   // delay(10000);
  //   // setLed(1,1);
  //   // dac.setPDBits(0000);

  //   delay(500);
  //   setLed(0,1);
  //   delay(500);
  // }

  sd = SunDial();


  float lon = 0;
  float lat = 0;
  uint32_t time = 0;

  while (1) {
    smartdelay(10);
    if (gps.location.lat() == 0) {
      lat = 30.266666f;
      lon = -97.733330f;
    }

    time = time + 100;  //(uint32_t) (gps.time.hour())  * 3600ul + gps.time.minute()*60+gps.time.second();
    // Serial1.println(time);
    // Serial1.println(gps.time.second());

    if (time == 0) {
      continue;
    }
    if (time >= 86400UL) {
      time = 0;
    }

    // sd.update(sd.convertDateToDays(gps.date.day(), gps.date.month()) , gps.time.hour()*3600+gps.time.minute()*60+gps.time.second(),
    //           gps.location.lat(), gps.location.lng(), gps.altitude.meters());


    sd.update(
      sd.convertDateToDays(23, 8),
      time,
      lat,
      lon,
      150);

    XYZ1931 XYZtarget = XYZ1931::XYZFromSpecDistFunc(&specDistFromSunDial);
    float cct = XYZtarget.getCCT();
    float weights[4];
    xyY colorTarget = CCT_To_xyY(cct);
    xyY clippedT = clipSignal(colorTarget, 3);
    // Serial1.print(cct);
    // Serial1.print(" ");
    // Serial1.print(colorTarget.x);
    // Serial1.print(" ");
    // Serial1.print(colorTarget.y);
    // Serial1.print("    ");
    // Serial1.print(clippedT.x);
    // Serial1.print(" ");
    // Serial1.print(clippedT.y);
    // Serial1.println();



    calculateWeightsForClippedTarget(colorTarget, weights, NUM_ILLUMINANTS);
    sprintf(buf, "utcT=%lu, T=%lu, cct = %lu", time, sd.UTCtoSolarLocal(time,sd.day, sd.longitude)/3600, (uint32_t) cct);
    Serial1.println(buf);
    for(uint8_t j=0; j<NUM_ILLUMINANTS; ++j){
      weights[j] *= sd.intensity;
      Serial1.print(weights[j]);
      Serial1.print(" ");
    }
    Serial1.println();
    setLed(0,1);
    dac.setAllChannelBrightness(weights);
  }
}

static void smartdelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < ms);
}

void loop() {
}






// SunDial s;
// XYZ1931 test;

// float specDistFromSunDial(float wavelen){
//     return s.atmosphericRadiator(wavelen);
// }

// int main()
// {
//     printf("wavelen,X,Y,Z\n");
//     s = SunDial();

//     for(int i=0; i<86400; i+=300){
//         uint32_t timehere = SunDial::UTCtoSolarLocal(i, 36, -97.733330);
//         int hour = timehere / 3600;
//         int minute = (timehere % 3600)/60;
//         int second = timehere %60;
//         s.update(216, i , 30.266666/180*M_PIf, -97.733330/180*M_PIf, 156);
//         printf("Austin's hour angle for %d:%d:%d is %f, intensity= %f\n", hour, minute, second, SunDial::hourAngle(i, 36, -97.733330), s.intensity);
//         test = XYZ1931(&specDistFromSunDial);
//         printf("airmass is %f, cct is %f\n", s.airMass, test.getCCT());
//     }
//     calculateTarget(myIlluminants[0]);
//     return 0;
// }
