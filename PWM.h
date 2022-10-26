#ifndef PWM_H
#define PWM_H
#include <Arduino.h>
#include "Constants.h"
#include "Pins.h"

class PWM
{
  public:
    PWM(void);
    void startPWM(void);
    void stopPWM(void);
    void setPWM(uint8_t val);
};

#endif
