#ifndef TIMER_H
#define TIMER_H
#include <Arduino.h>
#include "Constants.h"
#include "Pins.h"

class Timer
{
  public:
    Timer(void);
    void startTimer(void);
    void stopTimer(void);
};

#endif
