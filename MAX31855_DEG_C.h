/***************************************************************************
  Library for the MAX31855* thermocouple amplifier from Maxim Semiconductor
  Written by S George Matthews with contributions from Cory J. Fowler
             MAX31855@fickleview.com  Rev 1.1
  BSD license, all text above must be included in any redistribution.
 ***************************************************************************/

#ifndef  MAX31855_DEG_C_H
#define  MAX31855_DEG_C_H


#include "Arduino.h"


class MAX31855_DEG_C 
{
  public:
    MAX31855_DEG_C(int _CS);  // Chip Select pin
 

    bool dataAvailable(int _CS);  // Reads temperature raw data from chip and returns TRUE if data available
                                  // 0 no data


    int dataRead(byte _selectData);   // MUST call dataAvailable first to extract the raw data. Always returns decrees C
 
                               
  
 private:
    void ReadSPI(void);
    
    long  _data;
    long _extractedDataH;
    long _extractedDataC;
  
};

#endif // MAX31855_DEG_C_H