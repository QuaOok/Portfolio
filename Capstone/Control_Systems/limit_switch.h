#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include "Arduino.h"

class Switch {
public:
  // Constructor
  Switch(uint8_t pin, char application);

  // Initialize the button
  void begin();

  // Check for button events
  bool isPressed();
  bool checkReleased();
  void enableInterrupt();

private:
  uint8_t _pin;                 /** GPIO pin for the button */
  char _application;
  volatile bool _pressed;       /** Flag for button press */
  volatile bool _released;      /** Flag for button release */

  static void IRAM_ATTR handleInterrupt(void* arg); /** Interrupt handler */
};

#endif
