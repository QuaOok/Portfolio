#include "Stepper_Motor.h"

Motor::Motor(int pul, int dir, int steps)
{
	_PUL = pul; //set pin for PUL
	_DIR = dir; //set pin for DIR
	_steps = steps;
	_rpm = 1000;
	_running = false;
}

Motor::Motor(int pul, int dir, int ena, int steps)
{
	_PUL = pul; //set pin for PUL
	_DIR = dir; //set pin for DIR
	_ENA = ena;
	_steps = steps;
	_rpm = 1000;
}

void Motor::initialize() {
	pinMode(_PUL, OUTPUT);
  	pinMode(_DIR, OUTPUT);

  	digitalWrite(_DIR, LOW); // Set the motor to off initially
  	digitalWrite(_PUL, LOW); // Set the motor to off initially
}

/*
void Motor::set_rpm(int rpm)
{
	if (_steps == 0) {
        Serial.println("Error: Steps per revolution not defined.");
        return;
    }
    if (rpm <= 0) {
        Serial.println("Error: RPM must be greater than 0.");
        return;
    }

    _rpm = (60 * 1000000L) / (_steps * rpm);

    Serial.print("Speed set to: ");
    Serial.print(rpm);
    Serial.println(" RPM");

    Serial.print("Step delay: ");
    Serial.print(_rpm);
    Serial.println(" microseconds");
}
*/
void Motor::rotate(int angle)
{
	this->_running = true;
	int count = (angle / (360.0 / _steps));
	int actual = 0;
	for(int i = 0; i < count; i++)
	{
		if (!_running) { // Check if stop() was called
        	Serial.println("Rotation stopped early.");
        	break;
      	}

  		digitalWrite(_PUL, LOW);
  		delayMicroseconds(_rpm);
  		digitalWrite(_PUL, HIGH);
  		delayMicroseconds(_rpm);
  		++actual;
	}
	this->stop();
	Serial.println(actual);
}

void Motor::stop()
{
	_running = false;
	digitalWrite(_PUL, LOW);
}

void Motor::change_dir()
{
	int currentState = digitalRead(_DIR); // Read the current state of the pin
  	digitalWrite(_DIR, !currentState);   // Write the opposite state
}

void Motor::clockwise()
{
	digitalWrite(_DIR, LOW);	//Change the motor direction to clockwise;
}

void Motor::counter_clockwise()
{
	digitalWrite(_DIR, HIGH);	//Change the motor direction to counter clockwise
}

void Motor::run()
{
	digitalWrite(_PUL, LOW);
  	delayMicroseconds(_rpm);
  	digitalWrite(_PUL, HIGH);
  	delayMicroseconds(_rpm);
}
