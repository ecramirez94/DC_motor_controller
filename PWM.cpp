#include <Arduino.h>
#include "PWM.h"

/* Constructor */
PWM::PWM(void)
{
  TCCR2A = (1<<COM2A1) | (0<<COM2A0) | (0<<WGM21) | (1<<WGM20); /* PWM, Phase Correct, 122Hz frequency*/
  TCCR2B = (1<<WGM22) | (0<<CS22) | (0<<CS21) | (0<<CS20);
}

/* Member Functions */
void PWM::startPWM(void)
{
  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20);
}

void PWM::stopPWM(void)
{
  TCCR2B = (0<<CS22) | (0<<CS21) | (0<<CS20);
}

void PWM::setPWM(uint8_t val)
{
  OCR2A = val;
}
