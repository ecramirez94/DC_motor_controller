/*
 * Constants.h - File for firmware-wide Constants.
 * Created by Carlos Ramirez 05/13/2022
*/
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define RPM_ROW 25
#define PWM_ROW 45

/*
 * RPM_SCALE_FACTOR calculation:
 * #define CHOPPER_SLOTS 72
 * 
 * 1 / (CHOPPER_SLOTS / 60)
 */
#define RPM_SCALE_FACTOR 0.83

#define RPM_LABEL "RPM:"
#define PWM_LABEL "PWM Duty:"

#define RUNNING 0
#define STOPPED 1

#endif
