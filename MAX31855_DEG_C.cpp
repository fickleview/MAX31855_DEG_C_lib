/*************************************************************************** 
  Library for the MAX31855* thermocouple chip from Maxim Semiconductor
  Written by S George Matthews with starting contributions from Cory J. Fowler
              MAX31855@fickleview.com     Rev 1.1
  BSD license, all text above must be included in any redistribution.
 ***************************************************************************/

/*
http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf

D31   Sign - Thermal couple data
D30 to D18 - 14 bit hot Thermal couple data 12 + 2 decimal .25 /bit

D17 Reserved (ignore)

D16 Fault bit - high when any fault B0, B1, or B2 high

D15   Sign - Internal temperature
D14 to D4 - 12 bit cold Internal temperature 8 + 4 decimal .0625 / bit

D3 Reserved (ignore)

D2 Fault - High = Short to Vcc
D1 Fault - High = Short to GND
D0 Fault - High = Open thermal couple circuit

*/


#include "MAX31855_DEG_C.h"
#include <SPI.h>
#include <Streaming.h>

// When DEBUG_DATA defined, dataRead(byte _selectData) will return fake data cycling between high and low on every call
// _selectData == 4 return the HOT integer part
// _selectData == 3 return the HOT decimal part  0 to 3 
// dataAvailable(int _CS) always true
// It makes debugging you sketch much less painful.

//#define DEBUG_DATA  


MAX31855_DEG_C::MAX31855_DEG_C(int _CS) 
{

  SPI.begin();     // Pin 10 must be set as output if not used, otherwise SPI will not function
  
       pinMode(_CS, OUTPUT);
  digitalWrite(_CS, HIGH);
}


// Read the entire MAX31855 register which includes:
// 12 bits plus 2 decimal bits, hot junction (thermal couple), 
//  8 bits plus 4 decimal bits, cold junction  (internal) 
//  3 fault bits
//  WAIT AT LEAST 50mS between dataAvailable(int _CS) reads otherwise data will be faulty

#ifdef DEBUG_DATA
bool MAX31855_DEG_C::dataAvailable(int _CS) 
{
return true;
}

#else
bool MAX31855_DEG_C::dataAvailable(int _CS) 
{
  _data = 0;  // Always contains the entire raw 32 bit data read from MAX31855

  digitalWrite(_CS, LOW);        // Select the thermalcouple chip
  delayMicroseconds(1);

// Read 32 bits, 8 bits at a time
  for (int i = 0; i < 4; i++) 
  {
    _data = (_data << 8) + SPI.transfer(0x00);  // 8 bits at a time, mode 0, shifting it up as we read
  }

  digitalWrite(_CS, HIGH);      // De-Select the thermalcouple chip

 // Did we get any data?
 if(_data)                  
 {
  return true;
 }
 else
 {
 return false;
 }
}
#endif


// MUST call dataAvailable FIRST to extract the raw data. Always returns decrees C
// 0 Returns signed compensated Hot-External temperature in degrees C rounded to the nearest degree, no decimal bits
                                  
// 1 Returns the  Hot-External decimal temperature in degrees with no decimal bits.
// 2 Returns Cold-Internal with four decimal bits
// 3 Returns  Hot-External two decimal bits    0b01= .25, ob10= .50, 0b11= .75
// 4 Returns the fault data:
    // 4 Fault - High = Short to Vcc
    // 2 Fault - High = Short to GND
    // 1 Fault - High = Open thermal couple circuit   
                  

// returns 95 <> 700 in increments of 1

#ifdef DEBUG_DATA
int fakeData = 90;
int fakeDataHigh = 700;
int increment = 1;

int fakeDecimal = 0;
int decimalIncrement = 1;

int MAX31855_DEG_C::dataRead(byte _selectData)
{
if(_selectData == 3)
{
  fakeDecimal += decimalIncrement;
    if (fakeDecimal > 3)
    {
     fakeDecimal = 2;
     decimalIncrement = -1;
    }
	else if (fakeDecimal < 1)
	{
	fakeDecimal = 0;
     decimalIncrement = 1;
	}
  
      return fakeDecimal;   // Between 0 and 3
  }
   else
  {
  if(_selectData == 4)
  {
  return 0;
  }
  else
  {
   if (fakeData < 95)
   {
   increment = 1;
   }

  if (fakeData > fakeDataHigh)
  {
  increment = -1;
  }

  fakeData += increment;
  return fakeData;
  }
  }
}

#else

int MAX31855_DEG_C::dataRead(byte _selectData)
{
 if(_selectData == 4)
 {
   return (_data & 0x7);                    // Filter and return the lower three fault bits
  }
   else
  {
  // Hot Junction
  _extractedDataH = _data >> 18 ;             // D31 to D18 last two bits decimal part
  
  if(_selectData == 3)
  {
  return _extractedDataH & 0x3;
  }
   else
  {
  // Now the cold junction
  _extractedDataC = (_data & 0xFFF0) >> 4;  // D14 to D2 aligning last 4 bits for the decimal portion

  if(bitRead(_data,15))                     // D15 cold junction sign bit
  { 
    _extractedDataC += 0xFFFFF000;          // Make 8+4 bit a 32 bit number negative with leading ones
  }

  if(_selectData == 2)                       // Cold with four decimal bits
   {

  return _extractedDataC;
   }
    else
   {
   _extractedDataC = _extractedDataC >> 2;  // align the bits for a composite addition   
   

    if(_selectData == 1)
     {

      return _extractedDataH >> 2;             // Strip the decimal bits 
     }
      else
     {
    
      int _decBits = _extractedDataH & 0x3;    // Last two bits

      _extractedDataH = _extractedDataH >> 2;  // Strip the decimal bits   

      if (_decBits >= 2)                        // each bit = 0.25C
      {
       _extractedDataH += 1;                    // Round up
      }

       return _extractedDataH;
     }                  
   }
  }
 }
}
#endif

