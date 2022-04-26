#ifndef _PLATFORM_
#define _PLATFORM_

#include <Arduino.h>
#include <AsyncStepper.hpp>
#include <Adafruit_PWMServoDriver.h>

// PLATFORM
#define YAW_DIR PB11
#define YAW_STEP PB10
#define YAW_HOME_SENSE PA6
#define PITCH_DIR PB1
#define PITCH_STEP PB0
#define MOTOR_ENABLE PA7

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

#define Y_SPEED_MULTIPLIER 50 //(32 * 50) -> 1600
#define P_SPEED_MULTIPLIER 50 //
#define C_SPEED_MULTIPLIER 25


#define Y_HOMING_MASK 0b00000001
#define P_HOMING_MASK 0b00000010
#define C_HOMING_MASK 0b00000100

#define YAW_STEPS_PER_REV 6 * 200 * 4
#define PITCH_STEPS_PER_REV 2.5 * 200 * 4 
#define CAM_STEPS_PER_REV 6400

enum STATE {
  READY,
  HOMING,
  ENDSTOP_TRIGGERED,
  BUSY
};


namespace PLATFORM {
  struct platformState {
    int yawPos;
    int ptichPos;
    int camPos;
    int vBat;
    char laser : 1 ;    
  };

  typedef struct platformState platformState;

  byte _homing_ = 0;    
  byte state = READY;
  bool _laser = 0;
  bool l_solenoid_state = 0;
  bool r_solenoid_state = 0;
  unsigned long l_solenoid_on_time_us = 100;
  unsigned long r_solenoid_on_time_us = 100;

  AsyncStepper yawMotor(YAW_STEPS_PER_REV, YAW_DIR, YAW_STEP);
  AsyncStepper pitchMotor(PITCH_STEPS_PER_REV, PITCH_DIR, PITCH_STEP);
  
  Adafruit_PWMServoDriver ServoL = Adafruit_PWMServoDriver();

  #ifdef MOTORIZED_PI_CAM
    AsyncStepper camMotor(CAM_STEPS_PER_REV, CAMERA_MOTOR_DIR, CAMERA_MOTOR_STEP);
  #endif


  void setServo(byte pos) {
    
  }

  bool isMoving() {
    return true;
  }

  void ISR_y_home() {    
    Serial.println("Y_HOME_TRIGGERED!");

    if (!_homing_ & Y_HOMING_MASK) return;

    yawMotor.Stop();
    yawMotor.ResetAbsStep();
    _homing_ -= Y_HOMING_MASK;
    if (_homing_ == 0) //homing done
      state = READY;    
  }

  void ISR_p_home() {
    Serial.println("P_HOME_TRIGGERED!");
    if (!_homing_ & P_HOMING_MASK) return;

    // pitchMotor.Stop();
    // pitchMotor.ResetAbsStep();
    _homing_ -= P_HOMING_MASK;
    
    if (_homing_ == 0) //homing done
      state = READY;    
  }

  void ISR_c_home() {    
    #ifdef MOTORIZED_PI_CAM
    Serial.println("C_HOME_TRIGGERED!");
    if (!(_homing_ & C_HOMING_MASK)) return;
    camMotor.Stop();
    camMotor.ResetAbsStep();
    _homing_ -= C_HOMING_MASK;
    if (_homing_ == 0) //homing done
      state = READY;    
    #endif    
  }

  void begin() {
    //set pins
    pinMode(YAW_DIR, OUTPUT);
    pinMode(YAW_STEP, OUTPUT);    
    pinMode(YAW_HOME_SENSE, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(YAW_HOME_SENSE), ISR_y_home, RISING);
    
    pinMode(PITCH_STEP, OUTPUT);
    pinMode(PITCH_DIR, OUTPUT);
    pinMode(PITCH_HOME_SENSE, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PITCH_HOME_SENSE), ISR_p_home, RISING);

    #ifdef MOTORIZED_PI_CAM
      pinMode(CAMERA_MOTOR_STEP, OUTPUT);
      pinMode(CAMERA_MOTOR_DIR, OUTPUT);
      pinMode(CAMERA_MOTOR_SENSOR, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(CAMERA_MOTOR_SENSOR), ISR_c_home, RISING);
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

  AsyncStepper* _getStepper(char axis) {
    Serial.println("getting motor:");
    Serial.println(axis, HEX);

    if (axis == 0) {
      return &yawMotor;
    } if (axis == 1) {
      return &pitchMotor;
    } if (axis == 2) {
      #ifdef MOTORIZED_PI_CAM
      return &camMotor;
      #endif      
    }

    return nullptr;
  }

  /** Move an axis a number of steps at a given speed */
  void move (byte axis, bool dir, byte speed, byte steps) {    
    
    if (axis > 1) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }

    AsyncStepper *Motor = _getStepper(axis);

    if(Motor->State != AsyncStepper::Running) return; 

    if (speed != 0)
      Motor->SetSpeed(speed * Y_SPEED_MULTIPLIER);
    
    Motor->Rotate(steps, (dir)? AsyncStepper::CCW : AsyncStepper::CW);

  }

  void stopMovement(byte axis) {
    AsyncStepper *Motor = _getStepper(axis);
    Motor->Stop();
  }
  /** set the motor in a continually rotating movement at a given speed (as controlled by a joypad) */
  void setSpeed(byte axis, bool dir, byte speed) {
     if (axis > 1) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }
      Serial.print("[");
      Serial.print(axis, HEX);
      Serial.print(" - ");
      Serial.print(dir, HEX);
      Serial.print(" - ");
      Serial.print(speed, HEX);

      AsyncStepper *Motor = _getStepper(axis);

      Motor->SetSpeed(speed);
      Motor->RotateContinuous((dir)? AsyncStepper::CCW : AsyncStepper::CW);
  }

  /** rotate an axis to an absolute position as an angle  angle 
   *  angle is represented in 10ths of a degree so 90 degrees is 900
   * 
  */
  void setAngle(byte axis, bool dir, byte speed, int angle) {
    if (axis > 1) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }

    AsyncStepper *Motor = _getStepper(axis);

      if (speed != 0)
        Motor->SetSpeed(speed);
      
      yawMotor.RotateToAngle( angle / 10 ,(dir)? AsyncStepper::CCW : AsyncStepper::CW);    
  }

  void home(byte axis, byte spd) {
    if (axis > 1) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }

    Serial.println("Homing");
    if (state != STATE::READY) return;
    state = STATE::HOMING;
        
    if(axis == 0 ) { //YAW
      yawMotor.SetSpeed(spd * Y_SPEED_MULTIPLIER);      
      yawMotor.RotateContinuous(AsyncStepper::CW);
      _homing_ += Y_HOMING_MASK;


    } else if (axis == 1) { //PITCH
      yawMotor.SetSpeed(spd * P_SPEED_MULTIPLIER);      
      pitchMotor.RotateContinuous(AsyncStepper::CW);
      _homing_ += P_HOMING_MASK;
    
    } else if (axis == 2) { //CAMERA

    #ifdef MOTORIZED_PI_CAM
      cameraMotor.camMotor(spd * C_SPEED_MULTIPLIER);
      cameraMotor.RotateContinuous(AsyncStepper::CW);
      _homing_ +=C_HOMING_MASK;
    #endif 

    } else { //HOME ALL AXES
      _homing_ = C_HOMING_MASK + P_HOMING_MASK + Y_HOMING_MASK;
      
      yawMotor.SetSpeed(spd * Y_SPEED_MULTIPLIER);      
      yawMotor.RotateContinuous(AsyncStepper::CW);
      
      yawMotor.SetSpeed(spd * P_SPEED_MULTIPLIER);      
      pitchMotor.RotateContinuous(AsyncStepper::CW);
      
      #ifdef MOTORIZED_PI_CAM
      cameraMotor.camMotor(spd * C_SPEED_MULTIPLIER);
      cameraMotor.RotateContinuous(AsyncStepper::CW);
      _homing_ +=C_HOMING_MASK;
      #endif 
    }
  }

  void toggleLaser() {
    _laser = !_laser;
    digitalWrite(LASER, _laser);
  }

  void setLaser(bool on) {
    digitalWrite(LASER, !on);    
  }

  /** shoot a gun by toggling a solenoid on a given duration */
  void shoot(bool gun, int duration_ms ) {
    if (gun == 0) {
      l_solenoid_state = 1;
      l_solenoid_on_time_us = micros() + duration_ms * 1000;
    } else {
      r_solenoid_state = 1;
      r_solenoid_on_time_us = micros() + duration_ms * 1000;
    }
  }

  void update() {
    yawMotor.Update();
    pitchMotor.Update();
    #ifdef MOTORIZED_PI_CAM
    camMotor.Update();
    #endif

    //check for solenoid states and timing for closing them
    if (l_solenoid_state) {
      if (micros() > l_solenoid_on_time_us) {
        l_solenoid_state = 0;
      }
    }

    if (r_solenoid_state) {
      if (micros() > r_solenoid_on_time_us) {
        r_solenoid_state = 0;
      }
    }
    
    digitalWrite(GUN_L_SOLENOID, l_solenoid_state);
    digitalWrite(GUN_R_SOLENOID, r_solenoid_state);

  }

  void disableMotors() {
    digitalWrite(MOTOR_ENABLE, HIGH);
  }

  void enableMotors() {
    digitalWrite(MOTOR_ENABLE, LOW);
  }

  const char* getState() {
    return (String(yawMotor.GetAbsoluteStep()) + "|" +
    String(pitchMotor.GetAbsoluteStep()) +  "|" +
    String(_laser) + "|").c_str();
  }


  void loadAmmoSequence() {
    // unlock servo to allow barrel movement
    // rotate motor until it hits sensor
    // use same logic as for homing     
  }

  void lockAmmoSequence() {
    // move servo until it locks the barrel    
  }
}

#endif