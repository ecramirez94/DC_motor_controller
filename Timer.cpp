#include <Arduino.h>
#include "Timer.h"

/* Constructor */
Timer::Timer(void){}
  
void Timer::startTimer(void)
{
  TCCR0A = (1<<WGM01) | (0<<WGM00);
  TCCR0B = (0<<WGM02) | (1<<CS02) | (0<<CS01) | (1<<CS00);
  TIMSK0 = (1<<OCIE0A); /* Enable output compare match A interrupt */
  OCR0A = 154; /* Generate interrupt every 9.98ms (100.16Hz) */
}

void Timer::stopTimer(void)
{
  TCCR0B = (0<<CS02) | (0<<CS01) | (0<<CS00);
  TCNT0 = 0;
}
