
/*************************************************************************** 
  Library for the MAX31855* thermocouple chip from Maxim Semiconductor
  Written by S George Matthews with starting contributions from Cory J. Fowler
              MAX31855@fickleview.com     Rev 1.1
  BSD license, all text above must be included in any redistribution.
 **************************************************************************
 
This library uses integer only, no floating point to bog things down.
Only degrees C are returned. If you inhabit one of the two countries left in the WOrld that 
still use Fahrenght, then you can code your own conversions.

Despite that I return only integer, there are decimal bits available that you can print out.
See examples.


 */
 
 
#include <SPI.h>
#include <Streaming.h>   // Serial.print alternative
#include <MAX31855_DEG_C.h>

// Pin used for Chip Select typicaly 10 with mmost shields that use SPI, so select another pin
// Place MAX31855_DEG_C_lib folder in Arduino/Libraries. Restart Arduino to picup the new library

#define SS_PIN 6            // MAX31855 Thermal couple board select pin

 #define HOT_THERMO_SELECT           0   // Returns signed compensated Hot-External temperature in degrees C rounded to the nearest degree, no decimal bits
  #define HOT_THERMO_SELECT_NO_DEC   1   // Returns signed compensated Hot-External temperature in degrees C no decimal
 #define HOT_THERMO_SELECT_DEC       3  // Returns decimal bits.  1= .25, 2= .50, 3= .75

#define FAULT_THERMO_SELECT    4  // Returns the fault data as follows:
   #define FAULT_THERMO_SHORT_VCC 4
   #define FAULT_THERMO_SHORT_GND 2
   #define FAULT_THERMO_OPEN      1
   
#define DEBUG_TEMP

MAX31855_DEG_C ThermalCouple(SS_PIN);  

void setup() 
{
  Serial.begin(57600);
       pinMode(10, OUTPUT);    // If pin 10 is not used, it MUST be set as follows or SPI library will not work
  digitalWrite(10, HIGH);

  // put your setup code here, to run once:


}

void loop() 
{
  // put your main code here, to run repeatedly: 
  
       if(!(millis() % 1000))  // Every second
       {
        updateLEDtempDisplay();
       }
}

void updateLEDtempDisplay()
{
   if( ThermalCouple.dataAvailable(SS_PIN) )  // Always true for fake data
   {
    if (ThermalCouple.dataRead(FAULT_THERMO_SELECT))  //  Always 0 when fake data set in library
    {

     #ifdef DEBUG_TEMP
     Serial << F("ThermalCouple.dataRead(FAULT_THERMO_SELECT): ") << ThermalCouple.dataRead(FAULT_THERMO_SELECT) << endl;
     #endif
    }
    else
    {
      #ifdef DEBUG_TEMP
     Serial << F("ThermalCouple.dataRead(HOT_THERMO_SELECT): ") << ThermalCouple.dataRead(HOT_THERMO_SELECT) << endl;
     
     // Each decimal bit represents .25 degrees.
     Serial << F("With decimal: ") << ThermalCouple.dataRead(HOT_THERMO_SELECT_NO_DEC) << "." << 25 * ThermalCouple.dataRead(HOT_THERMO_SELECT_DEC) << endl << endl;

     #endif
    }
    
    
    }
    else  // Fault reported
    {
       #ifdef DEBUG_TEMP
       if(!(millis() % 1000))  // Every second
       {
       Serial << F("ThermalCouple.dataAvailable(SS_PIN) reports not available. Wired SPI up correctly? Try #define DEBUG_DATA in the library") << endl;
       #endif
     }
    }
}
