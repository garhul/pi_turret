
#include <Arduino.h>
#include <AsyncStepper.hpp>

#define YAW_STEPS_PER_REV 4800
#define PITCH_STEPS_PER_REV 2400

// PLATFORM
#define YAW_DIR PB11
#define YAW_STEP PB10
#define YAW_HOME_SENSE PA6
#define PITCH_DIR PB0
#define PITCH_STEP PA7
#define MOTOR_ENABLE PB1

#define PITCH_HOME_SENSE PA5
#define CURRENT_SENSE PA0

// GUNS
#define GUN_L_SERVO PA1
#define GUN_R_SERVO PA2

#define GUN_L_BARREL_MOTOR PA3
#define GUN_R_BARREL_MOTOR PA4

#define GUN_L_BARREL_SENSE PB5
#define GUN_R_BARREL_SENSE PB4

#define GUN_L_SOLENOID PB3
#define GUN_R_SOLENOID PA15

#define LASER PA12
#define PRESSURE_SENSE false


// pi mount motor ?
// #define MOTORIZED_PI_CAM
#define CAMERA_MOTOR_STEP false
#define CAMERA_MOTOR_DIR false;
#define CAMERA_MOTOR_SENSOR false;

#define Y_MAX_SPEED 1000
#define P_MAX_SPEED 1000
#define C_MAX_SPEED 500

enum STATE {
  READY,
  HOMING,
  ENDSTOP_TRIGGERED,
  BUSY
};

namespace MOTORS {
  boolean _homing = false;
  byte state = READY;

  AsyncStepper yawMotor(200, YAW_DIR, YAW_STEP);
  AsyncStepper pitchMotor(200, PITCH_DIR, PITCH_STEP);
  #ifdef MOTORIZED_PI_CAM
    AsyncStepper camMotor(200, CAMERA_MOTOR_DIR, CAMERA_MOTOR_STEP);
  #endif


  bool isMoving() {

  }

  void begin() {
    //set pins
    pinMode(YAW_DIR, OUTPUT);
    pinMode(YAW_STEP, OUTPUT);
    pinMode(YAW_HOME_SENSE, INPUT_PULLUP);

    pinMode(PITCH_STEP, OUTPUT);
    pinMode(PITCH_DIR, OUTPUT);
    pinMode(PITCH_HOME_SENSE, INPUT_PULLUP);

    #ifdef MOTORIZED_PI_CAM
      pinMode(CAMERA_MOTOR_STEP, OUTPUT);
      pinMode(CAMERA_MOTOR_DIR, OUTPUT);
      pinMode(CAMERA_MOTOR_SENSOR, INPUT_PULLUP);
    #endif

    pinMode(LASER, OUTPUT);
    pinMode(MOTOR_ENABLE, OUTPUT);

    pinMode(GUN_L_SOLENOID, OUTPUT);
    pinMode(GUN_R_SOLENOID, OUTPUT);
    pinMode(GUN_L_SERVO, OUTPUT);
    pinMode(GUN_R_SERVO, OUTPUT);
    
    pinMode(GUN_L_BARREL_SENSE, INPUT_PULLUP);
    pinMode(GUN_R_BARREL_SENSE, INPUT_PULLUP);
  }

  void move (byte axis, boolean dir, byte speed, byte steps) {

  }

  void setSpeed(byte axis, boolean dir, byte speed) {

  }

  void setAngle(byte axis, boolean dir, byte speed, int angle) {

  }

  void home(byte axis, byte spd) {
    if (state != STATE::READY) return;
    state = STATE::HOMING;
    
    //move until hitting the endstop (?);

    if(axis == 'Y') { //YAW
      // yawMotor.

    } else if (axis == 'P') { //PITCH

    } else if (axis == 'C') { //CAMERA

    } else { //HOME ALL AXES

    }

  }


  inline void _updateMotors() {
    // yawMotor.Update();
    // pitchMotor.Update();
  }

}