/***************************************************************************
* Example sketch for the BH1750_WE library
* 
* Mode selection / abbreviations:
* CHM:    Continuously H-Resolution Mode
* CHM_2:  Continuously H-Resolution Mode2
* CLM:    Continuously L-Resolution Mode
* OTH:    One Time H-Resolution Mode
* OTH_2:  One Time H-Resolution Mode2
* OTL:    One Time L-Resolution Mode
* 
* Measuring time factor:
* 1.0 ==> Measuring Time Register = 69
* 0.45 ==> Measuring Time Register = 31
* 3.68 ==> Mesuring Time Register = 254
* 
* Other implemented functions, not used in this example:
* resetDataReg() --> resets Data Register
* powerOn() --> Wake Up!
* powerDown() --> Sleep well, my BH1750
* 
* If you change the measuring time factor for calibration purposes, 
* then you need to divide the light intensity by the measuring time factor.
* 
* More information can be found on:
* https://wolles-elektronikkiste.de/en/bh1750fvi-gy-30-302-ambient-light-sensor
* or in German:
* https://wolles-elektronikkiste.de/bh1750fvi-lichtsensormodul
***************************************************************************/

#include <Wire.h>
#include <BH1750_WE.h>
#define BH1750_ADDRESS 0x23

BH1750_WE myBH1750(BH1750_ADDRESS); 
// You may also pass a TwoWire object like wire2 
// BH1750_WE myBH1750(&wire2, BH1750_ADDRESS);

void setup(){
  Serial.begin(9600);
  Wire.begin();
  myBH1750.init(); // sets default values: mode = CHM, measuring time factor = 1.0
  // myBH1750.setMode(CLM);  // uncomment if you want to change the default values
  // myBH1750.setMeasuringTimeFactor(0.45); // uncomment for selection of value between 0.45 and 3.68
}

void loop(){ 
  float lightIntensity = myBH1750.getLux();
  Serial.print(F("Light intensity: "));
  Serial.print(lightIntensity);
  Serial.println(F(" Lux"));
  delay(1000);
}
