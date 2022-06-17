#ifndef _PLATFORM_
#define _PLATFORM_

#include <Arduino.h>
#include <simplestepper.hpp>
#include <Servo.h>

// PLATFORM
#define YAW_DIR  PB11
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

#define Y_SPEED_MULTIPLIER 200 //(32 * 50) -> 1600 
#define P_SPEED_MULTIPLIER 200 //
#define C_SPEED_MULTIPLIER 100


#define Y_HOMING_MASK 0b00000001
#define P_HOMING_MASK 0b00000010
#define C_HOMING_MASK 0b00000100

#define YAW_STEPS_PER_REV 200 * 4
#define PITCH_STEPS_PER_REV 200 * 4 
#define CAM_STEPS_PER_REV 6400

#define HOME_DIRECTION 1

enum STATE {
  READY,
  HOMING,
  ENDSTOP_TRIGGERED,
  BUSY
};


enum PROPERTIES {
  ACCELERATION,
  DECELERATION,
  MAX_SPEED,
  SOFT_LIMIT_POSITION,
  CURRENT_POSITION,
};

namespace PLATFORM {

  byte _homing_ = 0;    
  bool l_barrel_seeking = false;
  bool r_barrel_seeking = false;
  byte state = READY;
  bool _laser = 0;
  bool l_solenoid_state = 0;
  bool r_solenoid_state = 0;

  unsigned long l_solenoid_on_time_us = 100;
  unsigned long r_solenoid_on_time_us = 100;

  SimpleStepper yawMotor(YAW_STEPS_PER_REV, YAW_DIR, YAW_STEP);
  SimpleStepper pitchMotor(PITCH_STEPS_PER_REV, PITCH_DIR, PITCH_STEP);
  #ifdef MOTORIZED_PI_CAM
    SimpleStepper camMotor(CAM_STEPS_PER_REV, CAMERA_MOTOR_DIR, CAMERA_MOTOR_STEP);
  #endif

  Servo ServoR;   
  Servo ServoL;   
  

  bool isMoving() {
    return true;
  }

  void ISR_y_home() {    
    Serial.println("Y_HOME_TRIGGERED!");

    // if (!_homing_ & Y_HOMING_MASK) return;

    yawMotor.stop();
    yawMotor.resetPosition();
    _homing_ -= Y_HOMING_MASK;
    if (_homing_ == 0) //homing done
      state = READY;    
  }

  void ISR_p_home() {
    Serial.println("P_HOME_TRIGGERED!");
    // if (!_homing_ & P_HOMING_MASK) return;
    //pitch needs to stop always at 0 and has a soft stop
    pitchMotor.stop();
    pitchMotor.resetPosition();
    _homing_ -= P_HOMING_MASK;
    
    if (_homing_ == 0) //homing done
      state = READY;    
  }

  void ISR_c_home() {    
    #ifdef MOTORIZED_PI_CAM
    Serial.println("C_HOME_TRIGGERED!");
    // if (!(_homing_ & C_HOMING_MASK)) return;
    camMotor.stop();
    camMotor.resetPosition();
    _homing_ -= C_HOMING_MASK;
    if (_homing_ == 0) //homing done
      state = READY;    
    #endif    
  }

  void ISR_l_barrel_pos() {      
    Serial.println("L_BARREL_INDEX");
    digitalWrite(GUN_L_BARREL_MOTOR, LOW);
    l_barrel_seeking = false;
  }

  void ISR_r_barrel_pos() {      
    Serial.println("R_BARREL_INDEX");
    digitalWrite(GUN_R_BARREL_MOTOR, LOW);
    r_barrel_seeking = false;
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

    attachInterrupt(digitalPinToInterrupt(GUN_L_BARREL_SENSE), ISR_l_barrel_pos, RISING);
    attachInterrupt(digitalPinToInterrupt(GUN_R_BARREL_SENSE), ISR_r_barrel_pos, RISING);
    
    pinMode(LASER, OUTPUT);
    pinMode(MOTOR_ENABLE, OUTPUT);

    pinMode(GUN_L_SOLENOID, OUTPUT);
    pinMode(GUN_R_SOLENOID, OUTPUT);
    pinMode(GUN_L_SERVO, OUTPUT);
    pinMode(GUN_R_SERVO, OUTPUT);
    
    ServoL.attach(GUN_L_SERVO); 
    ServoL.write(0);
    ServoR.attach(GUN_R_SERVO); 
    ServoR.write(0);

    pinMode(GUN_L_BARREL_SENSE, INPUT_PULLUP);
    pinMode(GUN_R_BARREL_SENSE, INPUT_PULLUP);
    //pitchMotor.setSoftLimit(pitchMotor.stepsPerRev * 1); //360 degrees movement
  }

  SimpleStepper* _getStepper(char axis) {
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

  void setServoPos(byte channel, byte pos) {
    if (channel == 0) {
      Serial.println("setting servoL to");
      Serial.println(map(pos, 0 , 63 , 0 , 65535));
      ServoL.write(map(pos, 0 , 63 , 0 , 65535));
    } else if (channel == 1) {
      Serial.println("setting servoR to");
      Serial.println(map(pos, 0 , 63 , 0 , 65535));
      ServoR.write(map(pos, 0 , 63 , 0 , 65535));
    }
  }

  /** Move an axis a number of steps at a given speed */
  void move (byte axis, bool dir, byte speed, byte steps) {    
    int computed_speed;
    if (axis > 1) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }

    SimpleStepper *Motor = _getStepper(axis);

    if(Motor->state != SimpleStepper::RUNNING) return; 

    computed_speed = ((dir) ? 1 : -1) * speed * Motor->stepsPerRev;
        
    if (computed_speed != 0)
      Motor->turn(steps, computed_speed / 16);
  }

  void stopMovement(byte axis) {
    SimpleStepper *Motor = _getStepper(axis);
    Motor->stop();
  }

  /** set the motor in a continually rotating movement at a given speed (as controlled by a joypad) */
  void setSpeed(byte axis, bool dir, byte speed) {
    int computed_speed; 
    
    if (axis > 1) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }                                                                                                                
    
    SimpleStepper *Motor = _getStepper(axis);
    computed_speed = ((dir) ? 1 : -1) * speed * Motor->stepsPerRev / 8;    
    Motor->setTargetSpeed(computed_speed);
  }

  /** rotate an axis to an absolute position as an angle  angle 
   *  angle is represented in 10ths of a degree so 90 degrees is 900
  */
  void setAngle(byte axis, bool dir, byte speed, int angle) {
    // TODO:: REWRITE
    long computed_speed;
    if (axis > 1) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }

    SimpleStepper *Motor = _getStepper(axis);

    if (speed != 0) {
      // Motor->setTargetSpeed(speed);
      // Motor->RotateToAngle( angle / 10 ,(dir)? SimpleStepper::CCW : SimpleStepper::CW);    
    }
  }

  void home(byte axis, byte spd) {
    if (axis > 3) {
      Serial.println("axis out of range");
      Serial.print(axis, HEX);
      return;
    }
   
    if (state != STATE::READY) return;
    state = STATE::HOMING;
    Serial.println("Homing");
    Serial.println(axis);
        
    if (axis == 3) {
      //HOME ALL AXES
      _homing_ = C_HOMING_MASK + P_HOMING_MASK + Y_HOMING_MASK;
      
      yawMotor.setTargetSpeed(HOME_DIRECTION * spd * yawMotor.stepsPerRev / 8);      
      pitchMotor.setTargetSpeed(HOME_DIRECTION * spd * pitchMotor.stepsPerRev / 8);

      #ifdef MOTORIZED_PI_CAM
      cameraMotor.setTargetSpeed(HOME_DIRECTION * spd * cameraMotor.stepsPerRev / 8) ;      
      #endif 
    
      return;
    }
    
    SimpleStepper *Motor = _getStepper(axis);    
    _homing_ += 1 << axis;
    Motor->setTargetSpeed(HOME_DIRECTION * spd * Motor->stepsPerRev / 8);
 
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
    yawMotor.update();
    pitchMotor.update();
    #ifdef MOTORIZED_PI_CAM
    camMotor.update();
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
    // platformState st = {
    //   .
    // }




    return (String(yawMotor.getPosition()) + "|" +
    String(pitchMotor.getPosition()) +  "|" +
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

  void setProperty(uint8_t axis, uint8_t property, uint16_t value) {
    SimpleStepper *Motor = _getStepper(axis);

    switch(property) {
      case ACCELERATION:        
        Motor->setAcceleration(value);
      break;
      case DECELERATION:
        Motor->setDeceleration(value);
      break;
      case MAX_SPEED:
        // not implemented
      break;
      case SOFT_LIMIT_POSITION:
        Motor->setSoftLimit(value);
      break;
      case CURRENT_POSITION:
        Motor->position = value;
      break;
    }

  }

  void nextDart(uint8_t chan) {    
    if (chan == 0) {
      if (l_barrel_seeking == true) return;
      digitalWrite(GUN_L_BARREL_MOTOR, HIGH);
      l_barrel_seeking = true;
    } else {
      if (r_barrel_seeking == true) return;
      digitalWrite(GUN_R_BARREL_MOTOR, HIGH);
      r_barrel_seeking = true;
    }
  }
}

#endif