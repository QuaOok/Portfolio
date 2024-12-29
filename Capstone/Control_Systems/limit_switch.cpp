#include "limit_switch.h"

extern volatile bool loadStart;
extern volatile bool cameraStart;

Switch::Switch(uint8_t pin, char application) 
{
  _pin = pin;
  _application = application;
  _pressed = false;
  _released = true;
}

void Switch::begin() {
  pinMode(_pin, INPUT_PULLDOWN);
  // Attach the interrupt to the limit switch pin, passing the object as an argument
  attachInterruptArg(digitalPinToInterrupt(_pin), handleInterrupt, this, RISING);
}

bool Switch::isPressed() {
  if (_pressed) {
    _pressed = false; // Reset the flag
    return true;
  }
  return false;
}

void IRAM_ATTR Switch::handleInterrupt(void* arg) {
  Switch* sw = static_cast<Switch*>(arg); // Get the object instance
  detachInterrupt(digitalPinToInterrupt(sw->_pin)); // Disable interrupt temporarily
  sw->_pressed = true;
  sw->_released = false;
  //need to stop motors by flag changing
  
  if(_application == 'c')
  {
    cameraStart = false;
  }
  if(_application == 'l')
  {
    loadStart = false;
  }
}


void Switch::enableInterrupt() {
  attachInterruptArg(digitalPinToInterrupt(_pin), handleInterrupt, this, RISING);
}

bool Switch::checkReleased() {
  if (digitalRead(_pin) == LOW) { // Check if the switch is released
    _released = true;
    _pressed = false;
    enableInterrupt(); // Re-enable the interrupt when the switch is released
  }

  return _released;
}
