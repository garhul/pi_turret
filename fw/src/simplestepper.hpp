#ifndef __SIMPLESTEPPER__
#define __SIMPLESTEPPER__

#define PULSE_WIDTH 5
#define DIR_CHANGE_SPEED 4000
#define STARTING_SPEED 200

#include <Arduino.h>


class SimpleStepper {
  
  public:
    enum States {
      STOPPED,
      RUNNING,
      ACCELERATING,
      BRAKING,
    };

    enum Modes {
      CONTINUOUS,
      DISTANCE
    };

    uint16_t stepsPerRev = 0;    
    float acceleration = 0.97;
    float deceleration = 1.03;
    int stepPin;
    int directionPin;
    
    bool direction = true;
    bool targetDirection = true;
    bool hardLimit = false;
    
    Modes mode = CONTINUOUS;
    
    uint32_t interval = 100; //running interval in µs    
    int targetSpeed;
    uint32_t lastUpdate = 0;
    
    uint32_t position = 0;
    uint32_t softLimitPosition = 0;
    
    int32_t targetPosition = 0;
    
    States state = STOPPED;

    SimpleStepper(uint16_t stepsPerRev, int directionPin, int stepPin) {
      this->stepPin = stepPin;
      this->directionPin = directionPin;
      this->stepsPerRev = stepsPerRev;      
    }

    void stop() {
      this->state = STOPPED;
      this->speed = 0;
      this->interval = (1e+6 / STARTING_SPEED) / 2;
    }
    
    void resetPosition() {
      this->position = 0;
    }

    void setPosition(uint32_t p) {
      this->position = p;
    }

    void setAcceleration(uint16_t val) { 
      if (val == 0) return;
      this->acceleration =  10000 / val; // TODO::map from 0.8 to 1.2 ;
    }

    void setDeceleration(uint16_t val) {
      if (val == 0) return;
      this->deceleration = 10000 / val;
    }

    void setHardLimit(bool en) {
      this->hardLimit = en;
    }

    void setSoftLimit(uint32_t limit) {
      this->softLimitPosition = limit;
    }
    
    /* returns position from 0 to stepsPerRev */
    uint16_t getPosition() {
      return this->position % this->stepsPerRev;
    }

    inline void _incspd() {
      this->state = ACCELERATING;
      this->interval *= this->acceleration;
    }

    inline void _decspd() {
      this->state = BRAKING;
      this->interval *= this->deceleration;
    }

    void update() {     
      if (this->state == STOPPED || ((micros() - this->lastUpdate) < this->interval))
        return;
 
      this->lastUpdate = micros();      
      if (abs(this->speed) < DIR_CHANGE_SPEED) { // we only allow change of direction at safe speeds
        if (this->targetSpeed == 0 ) {          
          this->stop();
          return;
        }

        this->direction = this->targetDirection;
        digitalWrite(this->directionPin, (this->direction)? HIGH : LOW);
      }

      this->speed =  (1e+6 / this->interval) * ((this->direction) ? 1 : -1);
      
      if (this->speed > 0) { // pos speed
        if (this->speed < this->targetSpeed ) {
          this->_incspd();
        }
  
        if (this->speed > this->targetSpeed) {
          this->_decspd();
        }

      } else if (this->speed < 0) { //neg speed
        if (this->speed > this->targetSpeed ) {
          this->_incspd();
        }

        if (this->speed < this->targetSpeed) {  
          this->_decspd();
        }
      }

      if ((this->softLimitPosition != 0) &&
          ((this->position >= this->softLimitPosition) && this->direction ) &&
          ((this->position <= this->softLimitPosition) && !this->direction)){
          Serial.println("Soft limit triggered");
          Serial.println(String(this->position) + " -- " + String(this->softLimitPosition));          
          this->state = STOPPED;
          return;
      }

      this->step(); 
    }

    /**
     * Set the turning speed in steps/s
     */
    void setTargetSpeed(int speed) {     
      this->mode = CONTINUOUS;
      this->targetDirection = (speed < 0) ? 0: 1;
      this->targetSpeed = speed;
      Serial.print("setting target speed ");
      Serial.println(this->targetSpeed);

      if (this->state == STOPPED) {
        this->state = ACCELERATING;
        this->interval = 1e+6 / STARTING_SPEED;
      }
      
      this->lastUpdate = 0;
    }

    /** 
     * Turn the motor in the given direction   
    **/
    void turn(uint16_t steps, int speed) {      
      this->targetDirection = (speed > 0) ? 1 : 0;        
      this->targetSpeed = speed;
      this->targetPosition = (speed > 0)? this->position + steps : this->position - steps;       
      

      // if it was stopped start accelerating
      if (this->state == STOPPED) {
        this->state = ACCELERATING;
        this->direction = this->targetDirection;
        digitalWrite(this->directionPin, (this->direction)? HIGH : LOW);
        this->interval = 1e+6 / STARTING_SPEED;  //start at dir change speed
      }

      this->lastUpdate = 0;
      
    }

  private:
    int16_t speed = 0;    
    inline void step() {
      digitalWrite(this->stepPin, HIGH);
      delayMicroseconds(PULSE_WIDTH);
      digitalWrite(this->stepPin, LOW);   
      // TODO: make position absolute;
      this->position+= (this->direction) ? 1 : -1 ; 
    }
};



#endif