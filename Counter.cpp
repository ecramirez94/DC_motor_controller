#include <Arduino.h>
#include "Counter.h"

/* Constructor */
Counter::Counter(void)
{
  TCCR1A = (0<<WGM11) | (0<<WGM10); // Set all WGM1x modes to 0 to ensure Normal mode. 
  TCCR1B = (0<<WGM12) | (0<<WGM13) | (1<<CS12) | (1<<CS11) | (1<<CS10); // External clk source, rising edge, on T1/PD5/Physical pin 11/Arduino pin 5
}

/* Member Functions */
void Counter::startCounting(void)
{
  /* ** First Reset Counter ** */
  TCNT1 = 0;

  /* ** Then Start Counting ** */
  TCCR1B = (1<<CS12) | (1<<CS11) | (1<<CS10);  /* Connect external clock source from counter, effectively turning on Timer/Counter1. */
}

void Counter::stopCounting(void)
{
  /* 
   * Disconnect clock source from counter, effectively turning off Timer/Counter1. 
   * Cannot simply disable counter from Power Reduction Register (PRR) since that would
   * make the data register inaccessable and thus wouldn't be able to collect the data. 
   */
  TCCR1B = (0<<CS12) | (0<<CS11) | (0<<CS10);
}
