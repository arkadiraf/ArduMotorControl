/* ArduMoto Control 09/09/2020

   Arduino Uno
   Sparkfun Ardumoto shield Dev-14129
   
*/
#include "ArduMotoControl.h"

// define constants
#define DT_CONTROL_MILLIS 10 // Control DT suggested range 5-50 mSec --> 200-20 Hz

// Control Variables
float motorRPMdt = 0;
float motorRPMPulses = 0;

// control loop function
void MotorControlLoop(void);
// Control function
float MotorControl(float desiredCMD);


// Init
void setup() {
  // intinitalize encoder
  InitArduMotorControl();
  pinMode(LED_BUILTIN, OUTPUT);

  // init serial
  Serial.begin (115200);
  
  //////////////////////////////
  //////// Student Code ////////
  //////////////////////////////

  // serial plotter legend
  Serial.println("DesireCMD, MotorCMD, Error, RPMdt, MCUload");
  
  //////////////////////////////
  //// End of student Code /////
  //////////////////////////////
}

void loop() {
  static unsigned long millisTick = millis();
  if ((millis() - millisTick) >= DT_CONTROL_MILLIS){
    millisTick = millis();
    unsigned long microsTick = micros(); // simple loop cpu consumption measurement
    digitalWrite(LED_BUILTIN,HIGH);

    // update Motor RPM variables
    motorRPMdt = ReadRPM();
    motorRPMPulses = readRPMEncoder();
    
    //////////////////////////////
    //////// Student Code ////////
    //////////////////////////////

    // motor control loop function call
    MotorControlLoop();

    //////////////////////////////
    //// End of student Code /////
    //////////////////////////////

    //debug code consumption better alternativelyy measure d13 duty cycle / frequency
    Serial.print(", ");
    Serial.println((micros()-microsTick)/100); // value in % for control loop
    digitalWrite(LED_BUILTIN,LOW);
  }
}

//////////////////////////////
////// Student functions /////
//////////////////////////////

// control loop function
void MotorControlLoop(void){
  // Command profile
  float desiredCMD = 100 * SineCMD(0.5);  // Sine command, input Hz
  
  // motor control function
  float motorCMD = MotorControl(desiredCMD);
  
  // update motor, values range of [-1..1]
  motorComand(motorCMD);
  
  // send values to serial plotter
  Serial.print(desiredCMD);
  Serial.print(", ");
  Serial.print(motorCMD*100); // scaled to 100% to be visible on plot
  Serial.print(", ");
  Serial.print(desiredCMD - motorRPMdt);
  Serial.print(", ");
  // Serial.print(motorRPMPulses); // motorRPMPulses
  Serial.print(motorRPMdt); // motorRPMPulses
} 

// control loop function
float MotorControl(float desiredCMD){
  // initialize variables
  static float dt = DT_CONTROL_MILLIS / 1000.0;
  static float ciEr = 0; // integral error
  static float cdEr = 0; // differamtial error
  static float cEr  = 0; // current error
  static float lEr  = 0; // last error
  
  // control coefficients
  float kp = 0.002;
  float ki = 0.05;
  float kd = 0;
  
  // update error
  lEr = cEr;
  cEr = desiredCMD - motorRPMdt;
  ciEr = ciEr + cEr*dt;
  cdEr = (cEr- lEr) / dt;
  
  // clip integrator error
  if (ciEr*ki > 2.5) ciEr = 2.5/ki;
  if (ciEr*ki < -2.5) ciEr = -2.5/ki;

  // update control command
  float motorCMD = kp*cEr + ki*ciEr + kd*cdEr;
  return motorCMD;
}

//////////////////////////////
// End of Student functions //
//////////////////////////////
 
