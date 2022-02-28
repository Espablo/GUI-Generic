/*****************************************
This is a library for the BH1750FVI Lightsensor Module

You'll find an example which should enable you to use the library. 

You are free to use it, change it or build on it. In case you like it, it would be cool 
if you give it a star.

If you find bugs, please inform me!

Written by Wolfgang (Wolle) Ewald
https://wolles-elektronikkiste.de

*******************************************/


#ifndef BH1750_WE_H
#define BH1750_WE_H

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <Wire.h>

#define DATA_REG_RESET 0b00000111   
#define POWER_DOWN 0b00000000   
#define POWER_ON 0b00000001   

enum BH1750Mode {
    CHM =     0b00010000,   //CHM: Continuously H-Resolution Mode
    CHM_2 =   0b00010001,   //CHM_2: Continuously H-Resolution Mode2
    CLM =     0b00010011,   //CLM: Continuously L-Resolution Mode
    OTH =     0b00100000,   //OTH: One Time H-Resolution Mode
    OTH_2 =   0b00100001,   //OTH_2: One Time H-Resolution Mode2
    OTL =     0b00100011    //OTL: One Time L-Resolution Mode
};


class BH1750_WE{
    public:
        BH1750_WE(uint8_t addr);
        BH1750_WE(TwoWire *w, uint8_t addr);
        void init();
        void setMeasuringTimeFactor(float f);
        void setMode(BH1750Mode d_mode);
        float getLux();
        void resetDataReg();
        void powerOn();
        void powerDown();
                    
    private:
        void setI2C_Address(uint8_t addr);
        void writeBH1750_WE(uint8_t);
        uint16_t readBH1750_WE();
        TwoWire *_wire;
        float lux;
        int I2C_Address;
        float mtf;      //measuring time factor
        BH1750Mode deviceMode;
};

#endif

