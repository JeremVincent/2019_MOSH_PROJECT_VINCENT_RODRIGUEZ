#ifndef GAS_H
#define GAS_H

#include <Arduino.h>

class GasSensor {

  public:

  GasSensor();

  void calibration();

  float readValue();

  
  private:
  
  float R0_m;
  
};

#endif
