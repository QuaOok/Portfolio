// this is special doxygen format commands
/*! @mainpage Crack Detection Controller Index Page
 *
 * \section intro_sec Introduction
 *
 * The Crack Detection Controller is the embedded control system to be used with the Crack Detection System code that runs on a PC. The PC code provides the user interface and analysis functions for the system. This embedded controller interfaces to the load cell, motors and limit switches needed to control the crack detection hardware.
 *
 * \todo 
 * 1. Add printout of current_Load after start
 * 2. Add printout of current_Load even after reaching at the desired_Load
 * 3. Add function to remove the force (Things to do later)
 * 4. Add a function when a value is outside of its range. 
 * 5. Add a function to stop and resume just a camera
 * 6. Add a function to stop and resume just force application

 * \section author Developed By
 * Students in The Design Lab at Rensselaer, Fall 2024
 *
 */
 
#include <Stepper_Motor.h>
#include <limit_switch.h>
#include "HX711.h"
#include <vector>


#define LOADCELL_DOUT_PIN 14  /** DAT pin for Loadcell amplifier */
#define LOADCELL_SCK_PIN 12   /** SCK pin for Loadcell amplifier */


#define PUL_L 19 /** Pulse pin on the stepping motor controller for the Force application */
#define DIR_L 18 /** Direction pin on the stepping motor controller for the Force application */

#define PUL_C 23 /** Pulse pin on the stepping motor controller for the Camera application */
#define DIR_C 22 /** Direction pin on the stepping motor controller for the Camera application */

#define steps_L 400 /** Steps for the stepping motor controller for the Force application */
#define steps_C 6400 /** Steps for the stepping motor controller for the Camera application */

#define SWITCH_TOP 1 /** Pins for Top limit switch for Force applicaiton */
#define SWITCH_BOTTOM 2 /** Pins for Bottom limit switch for Force applicaiton */
#define SWITCH_LEFT 3 /** Pins for Left limit switch for Camera applicaiton */
#define SWITCH_RIGHT 4 /** Pins for Top limit switch for Force applicaiton */

long current_Load; /** Current reading of the load cell */
long desired_Load;
int length = 0;
int camera_Moved = 0;
char s[80];                       // used for printing

volatile bool loadStart = false; /** Flag to control motor state in Force Application */ 
volatile bool cameraStart = false; /** Flag to control motor state in Camera Application */

volatile bool ready = false; /** Flag to indicate the system is ready */
volatile bool reset = true; /** Flag to reset the system */

String current_command;

// Initialize the load cell
HX711 LoadCell;

// Initialize the stepper motor
//Motor motor_L(PUL_L, DIR_L, steps);
Motor motor_C(PUL_C, DIR_C, steps_C);
Motor motor_L(PUL_L, DIR_L, steps_L);

// Initialize the limit switches
Switch Switch_Top(SWITCH_TOP);
Switch Switch_Bottom(SWITCH_BOTTOM);
Switch Switch_Left(SWITCH_LEFT);
Switch Switch_Right(SWITCH_RIGHT);

//prototype functions
void setting_motors();
void onUartReceive();
std::vector<String> parseCommand(const String& command);

/**
 * Setup initailizes two motors, four limit switches, and the loadcell.
 * Initialize communication using Serial interrupt
 */
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("booting");
  motor_L.initialize(); 
  motor_C.initialize();
  Serial.println("motors setup");
  LoadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("loadcell started");
  Switch_Top.begin();
  Switch_Bottom.begin();
  Switch_Left.begin();
  Switch_Right.begin();

  Serial.println("Enter command:");

  Serial.onReceive(onUartReceive);                // Enable RX interrupt
}

void loop() {
  if(reset)
  {
    setting_motors(); //relocate motors
    ready = true;
  }

  if(ready)
  {
    //read load cell
    Serial.println("checking load cell");
    while(! LoadCell.is_ready()); //wait until loadcell value is ready
    Serial.println("load cell ok...");
    current_Load = LoadCell.read();
    sprintf(s, "current load=%f", current_Load);
    Serial.println(s);
    if(loadStart) //run force_application motor if the flag is up
    {
      if(current_Load <= desired_Load) //run motor if desired load is not reached
      {
        Serial.println("motor L run");
        motor_L.run();
      }
    }
    if(cameraStart) //run camera_position motor if the flag is up
    {
      //move certain length
      //then cameraStart = false;
      if(camera_Moved < length) //run motor if the camera hasn't moved enough
      {
        motor_C.run();
        ++camera_Moved;
        Serial.println("motor C run");
      }
      else
      {
        camera_Moved = 0;
        cameraStart = false;
        Serial.println("motor C NOT run");
      }
    }
  }
}

/**
 * Relocate both motors to the starting point
 */
void setting_motors() //function that relocate both motors to the starting point
{
  if(Switch_Bottom.checkReleased())
  {
    //motor_L.clockwise or counter clockwise
    while(!Switch_Bottom.isPressed())
    {
      motor_L.run();
    } 
    motor_L.change_dir();
  }
  Switch_Top.checkReleased();

  if(Switch_Left.checkReleased())
  {
     //motor_C.clockwise or counter clockwise
    while(!Switch_Left.isPressed())
    {
      motor_C.run();
    } 
    motor_C.change_dir();
  }
  Switch_Right.checkReleased();
  Serial.println("Ready to Start");

  reset = false; //relocating is done
}

/**
 * Reads command from GUI
 */
void onUartReceive() {
  while (Serial.available() > 0) {
    String com = Serial.readStringUntil('\n'); // Read incoming command
    com.trim(); // Remove leading/trailing whitespace
    std::vector<String> command = parseCommand(com);
    
    current_command = command[0];

    if (current_command == "start") {
      desired_Load = command[1].toInt();
      loadStart = true; // Set the flag to start the motor in Force Application
    } else if (current_command == "stop") {
      loadStart = false; // Clear the flag to stop the motor in Force Application
      cameraStart = false;
      Serial.println("Motor stopped!");
    } else if (current_command == "resume") {
      loadStart = true;
    } 
    else if (current_command == "camera") {
      length = command[1].toInt();
      cameraStart = true; //Set the flag to start the motor in Camera Position
    } 
    else if (current_command == "reset") {
      //reset flag up;
      ready = false;
      reset = true;
      Serial.println("reset");
    } 
    else if (current_command == "loadcell") {
      //send current loadcell value
      Serial.println(current_Load);
    }
    else if (current_command == "temperature") {
      //read temperature
      //send current temperature value
      //float temperature = getTemperature();
      //Serial.println("Temperature: " + String(temperature) + " °C");
    }
    else {
      Serial.println("Unknown command: " + command[0]);
    }
  }
}

/**
 * Parse the command from GUI to read
 */
std::vector<String> parseCommand(const String& command) {
  std::vector<String> parts;

  if (command.indexOf(':') == -1) {
    // No delimiter present; treat the whole string as one command
    parts.push_back(command);
  } 
  else {
    // Delimiter found; split the command into parts
    int start = 0;
    int end = command.indexOf(':');

    while (end != -1) {
      parts.push_back(command.substring(start, end));
      start = end + 1;
      end = command.indexOf(':', start);
    }

    // Push the last part
    parts.push_back(command.substring(start));
  }

  return parts;
}
