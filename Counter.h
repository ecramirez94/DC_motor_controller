#ifndef COUNTER_H
#define COUNTER_H
#include <Arduino.h>
#include "Constants.h"
#include "Pins.h"

class Counter
{
  public:
    Counter(void);
    void startCounting(void);
    void stopCounting(void);
};

#endif
