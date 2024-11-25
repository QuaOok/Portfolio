#include <Motor_Capstone.h>

#define PUL 19 //PUL
#define DIR 18 //DIR
#define ENA 5 //ENA
#define steps 400


// initialize the stepper lib
Motor motor(PUL, DIR, steps);

void setup() {
  motor.initialize();
  Serial.begin(115200);
  Serial.println("Enter angle:");
}


void loop() {
  if (Serial.available() > 0) {
    // Read the input as an integer
    int inputNumber = Serial.parseInt();

    // Echo the number back to the Serial Monitor
    Serial.print("You entered: ");
    Serial.println(inputNumber);

    // Example: Perform an action based on the input
    if (inputNumber == 90) {
      Serial.println("Rotate 90 degree");
      motor.rotate(90);
    } 
    else if (inputNumber == 180) {
      Serial.println("Rotate 180 degree");
      motor.rotate(180);
    } 
    else if (inputNumber == 270) {
      Serial.println("Rotate 270 degree");
      motor.rotate(270);
    } 
    else if (inputNumber == 360) {
      Serial.println("Rotate 360 degree");
      motor.rotate(360);
    } 
    else if (inputNumber == 1) {
      motor.change_dir();
      Serial.println("Direction is changed.");
    }
    else {
      Serial.println("Unknown number.");
    }
  }
}

/*
void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read incoming command
    command.trim();  // Remove any trailing new lines or spaces

    //std::vector<String> parts = parseCommand(command);

    for (const String& part : parts) {
      Serial.println(part);
    }
  }  
}
*/
/*
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
*/
