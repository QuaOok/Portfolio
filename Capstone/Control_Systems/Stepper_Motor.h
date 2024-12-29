#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include "Arduino.h"

class Motor
{
  public:
    Motor(int pul, int dir, int steps);
    Motor(int pul, int dir, int ena, int steps);
    void initialize();
    //void set_rpm(int rpm);
    void rotate(int angle);
    void stop();
    void change_dir();
    void clockwise();
    void counter_clockwise()
    void run();

  private:
    int _steps;
    int _PUL;
    int _DIR;
    int _ENA;
    int _rpm;
    int _running;
};
