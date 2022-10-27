#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Constants.h"
#include "Counter.h"
#include "Pins.h"
#include "PWM.h"
#include "Timer.h"

volatile uint8_t state = STOPPED;
volatile bool remote = false;
bool update_OLED = true;
volatile uint16_t rpm = 0;
volatile uint8_t count = 0;

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Counter Counter;
PWM PWM;
Timer Timer;

void setup() 
{
  pinMode(PWM_PIN, INPUT);
  pinMode(COUNTER_PIN, INPUT_PULLUP);
  pinMode(PWM_OUTPUT, OUTPUT);
  pinMode(START_STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(WINDOW_TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_OUT_PIN, OUTPUT);
  pinMode(REMOTE_CONTROL_PIN, INPUT_PULLUP);
  digitalWrite(TRIGGER_OUT_PIN, HIGH);

  /* 
   *  Setup Interrupt Requests
   *  Start/Stop button on INT0 (Pin 2)
   *  Window Trigger Signal on INT1 (Pin 3)
   */
  EICRA = (0<<ISC11) | (1<<ISC10) | (1<<ISC01) | (0<<ISC00); /* Logical change on INT1, Falling edge of INT0 */
  EIMSK = (1<<INT1) | (1<<INT0);  /* Enable external interrupt requests 0 and 1. */

  /*
   * Pin change interrupts
   */
  PCICR = (1<<PCIE2); // Enable Pin Change Interrupts on pin bank: PCINT[23:16]
  PCMSK2 = (1<<PCINT23); // Enable Pin Change Interrupt on Arduino pin 7 (REMOTE_CONTROL_PIN)

  Timer.startTimer();
  Counter.startCounting();
  PWM.startPWM();

  setupOLED();
  drawFrameOLED();

  updateRPM(0);
  updatePWM(0);
}

void loop() 
{
  switch (state)
  {
    case STOPPED:
      if (update_OLED)
      {
        PWM.setPWM(0); /* Stop PWM */
        
        if (remote)
          updateRPM(F("Remote Stop"));
        else
          updateRPM(F("Stop"));
          
        update_OLED = false;
      }
    break;
    case RUNNING:
      updateRPM(rpm);
      updatePWM(analogRead(PWM_PIN));
    break;
    default:

    break;
  }
}

/* Interrupt Service Routines */
ISR(INT0_vect)
{
  if (state == RUNNING)
    state = STOPPED;
  else
  {
    state = RUNNING;
    remote = false;
  }

  update_OLED = true;
}

ISR(INT1_vect)
{
  /* 
   *  Each time the WINDOW_TRIGGER_PIN is toggled
   *  by the chopper wheel, generate a pulse on 
   *  the TRIGGER_OUT_PIN.
   */
  bitClear(PORTD, PD4);
  bitSet(PORTD, PD4);
}

ISR(TIMER0_COMPA_vect)
{
  /* Collect pulses for 1 second */
  if (count++ >= 100)
  {
    rpm = uint16_t(float(TCNT1) * RPM_SCALE_FACTOR);
    TCNT1 = 0;
    count = 0;
  }
}

ISR(PCINT2_vect)
{
  bool remote_control = digitalRead(REMOTE_CONTROL_PIN);

  if (remote_control) /* If the MODE_SELECT pin was released (and pulled HIGH via internal pull-up resistor) */
  {
    state = STOPPED;
    remote = true;
  } else
    state = RUNNING;

  update_OLED = true;
}
/* End Interrupt Service Routines */



/* OLED Display Functions */
void setupOLED(void)
{
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
}

void drawFrameOLED(void)
{
  displayCenterOfRow("Chopper Controller", 5);
  oled.drawRect(1, 1, oled.width() - 1, oled.height() - 1, WHITE);
  oled.setCursor(5, RPM_ROW);
  oled.println(RPM_LABEL);
  oled.setCursor(5, PWM_ROW);
  oled.println(PWM_LABEL);
  oled.display();
}

void displayCenterOfRow(String text, uint8_t row) 
{
  uint8_t len = calcTextLength(text);

  // display on horizontal and vertical center
  oled.setCursor((SCREEN_WIDTH - len) / 2, row);
  oled.println(text); // text to display
  oled.display();
}

uint8_t calcTextLength(String text)
{
  int16_t x1;
  int16_t y1;
  uint16_t width = 0;
  uint16_t height = 0;

  oled.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  return width;
}

void updateRPM(uint16_t rpm)
{
  uint8_t len = calcTextLength(RPM_LABEL);
  /*
   * fillRect clears the area where old digits were to make room for new digits
   * 
   * oled.fillRect(start_x_coor, start_y_coor, width_of_rect, height_of_rect);
   * 
   * - start_x_coor and start_y_coor defines top left corner of rectangle
   * - Each number is 5 pixels wide plus 1 pixel between numbers -> 5 numbers * 6 pixels = 30 pixels wide
   */
  oled.fillRect(len + 10, RPM_ROW, 70, 8, BLACK); 
  oled.setCursor(len + 10, RPM_ROW); /*10 is space between colon and RPM value*/
  oled.println(rpm);
  oled.display();
}

void updateRPM(String notice)
{
  uint8_t len = calcTextLength(RPM_LABEL);

  /*
   * fillRect clears the area where old digits were to make room for new digits
   * 
   * oled.fillRect(start_x_coor, start_y_coor, width_of_rect, height_of_rect);
   * 
   * - start_x_coor and start_y_coor defines top left corner of rectangle
   * - Each number is 5 pixels wide plus 1 pixel between numbers -> 5 numbers * 6 pixels = 30 pixels wide
   */
  oled.fillRect(len + 10, RPM_ROW, 70, 8, BLACK); 
  oled.setCursor(len + 10, RPM_ROW); /*10 is space between colon and RPM value*/
  oled.println(notice);
  oled.display();
}

void updatePWM(uint16_t pwm)
{
  pwm = map(pwm, 0, 1023, 0, 255);
  PWM.setPWM(pwm);
  pwm = uint16_t((float(pwm) / 255.0) * 100.0);
  uint8_t len = calcTextLength(PWM_LABEL);

  /*
   * fillRect clears the area where old digits were to make room for new digits
   * 
   * oled.fillRect(start_x_coor, start_y_coor, width_of_rect, height_of_rect);
   * 
   * - start_x_coor and start_y_coor defines top left corner of rectangle
   * - Each number is 5 pixels wide plus 1 pixel between numbers -> 4 numbers * 6 pixels = 24 pixels wide
   */
  oled.fillRect(len + 10, PWM_ROW, 24, 8, BLACK); 
  oled.setCursor(len + 10, PWM_ROW); // 10 is space between colon and PWM value
  oled.print(pwm);
  oled.println("%");
  oled.display();
}

/* End OLED Display Functions */ 
