#ifndef _COMMAND_DISPATCHER_
#define _COMMAND_DISPATCHER_

#include <platform.hpp>
#include <types.h>
#define BUFF_SIZE 8

#define CHANNEL_MASK 0b11000000
#define SERVO_CHANNEL_MASK 0b11000000
#define HOME_SPEED_MASK 0b00111111
#define MOVE_SPEED_MASK 0b00011111
#define DIR_MASK 0b00100000


//******************************* I2C Codes samples **********************************//

// H -> Homing move (? for all axes), spd is in units, direction is ignored
// M -> Linear move, units represent amount of steps, spd is used here for speed of movement
// A -> Acceleration move, units represent speed
// R -> Radial move, move by *units* degrees, at spd speed
// D (disable motors)


// Gun commands
// Base Command | value | [extra_val]
// S/L/


// S L 100 (Shoot Left gun (open solenoid) for 100 ms )
// S R 50  (shoot right gun (open solenoid for 50 ms))
// L 1  (Laser on, L0 Laser off)
// reload gun? 

// Extra commands
//? => get state
//? V Requests info from sensors (current, voltage, pressure)
//? M Requests motor position information
//? L Requests Lidar info ? ?  ? ?
//$A => set acceleration for motors ?

enum CMDS {
  NONE, // 0x00
  // MOTOR PROPERTIES
  SET_PROPERTY,         //0x01  
  // MOTOR MOVEMENT CONTROL
  DISABLE_MOTORS,       //0x02
  ENBALE_MOTORS,        //0x03
  MOVE_STEPS,           //0x04
  MOVE_TO_ANGLE,        //0x05
  MOVE_UNTIL_LIMIT,     //0x06
  SET_TARGET_SPEED,     //0x07
  STOP,                 //0x08
  HOME,                 //0x09
  
  // GUNS RELATED CONTROL
  SET_LASER_STATE,      //0x0A
  BARREL_NEXT_DART,     //0x0B
  ENABLE_SOLENOID_MS,   //0x0C
  SET_SERVO_POSITION,   //0x0D
};

namespace COMMAND_DISPATCHER {
 

  void dispatch(command c) {
    movementPayload mp = {
      .speed = static_cast<uint16_t>(c.payload[1] << 8 + c.payload[2]),
      .dir = static_cast<bool>(c.payload[0] & 1),
      .distance = static_cast<uint16_t>((c.payload[3] << 8 + c.payload[4]))
    };
    
    propertyPayload p = {
      .propertyId = static_cast<uint8_t>(c.payload[1]),
      .value = static_cast<uint16_t>((c.payload[2] << 8) + c.payload[3])
    };

    switch (c.cmd) {
      case CMDS::SET_PROPERTY:        
        PLATFORM::setProperty(c.channel, p.propertyId, p.value);
      break;
    
      case CMDS::DISABLE_MOTORS: //disable steppers, reenable with any other command
        Serial.println("Disabling motors");
        PLATFORM::disableMotors();        
      break;

      case CMDS::ENBALE_MOTORS: //enable steppers,
        Serial.println("Enabling motors");
        PLATFORM::enableMotors();      
      break;
      
      // ---------------------------------------------------------- //
      //                Gun related commands                     //
      // ---------------------------------------------------------- //

      case CMDS::SET_LASER_STATE: //toggle laser
        Serial.println("Laser");
        PLATFORM::setLaser(c.payload[0] & 1);        
      break;   

      case CMDS::ENABLE_SOLENOID_MS: 
        Serial.println("Shooting gun");        
        PLATFORM::shoot(c.channel, c.payload[0] * 10); // every 1 unit represents 10ms, so range goes from 10 ms to 2550 ms
      break;

      case CMDS::SET_SERVO_POSITION:
        Serial.println("Setting servo position");        
        PLATFORM::setServoPos(c.channel, c.payload[1]);
      break;

      // ---------------------------------------------------------- //
      //                Motor movement commands                     //
      // ---------------------------------------------------------- //
      case CMDS::STOP:
        Serial.println("Stopping");        
        PLATFORM::stopMovement(c.channel);
      break;

      case CMDS::MOVE_STEPS: //Move motor by steps 
        Serial.println("Advancing steps");        
        PLATFORM::move(c.channel, mp.dir, mp.speed, mp.distance);
      break;

      case CMDS::HOME:
        Serial.println("Homing");     
        PLATFORM::home(c.channel, mp.speed);
      break;

      case CMDS::SET_TARGET_SPEED: // Continue motor movement by altering their current speed        
        Serial.println("Accelerating");     
        PLATFORM::setSpeed(c.channel, mp.dir, mp.speed);      
      break;

      case CMDS::MOVE_TO_ANGLE: // Radial move to an specific angle       
        Serial.println("move to angle") ;        
        PLATFORM::setAngle(c.channel, mp.dir, mp.speed, mp.distance);
      break;

      // ---------------------------------------------------------- //
      //                Gun movement commands                       //
      // ---------------------------------------------------------- //
      case CMDS::BARREL_NEXT_DART:
        Serial.println("Loading next dart");
        PLATFORM::nextDart(c.channel);
      break;
    }
  }
}

#endif