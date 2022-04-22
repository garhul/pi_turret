#ifndef _PARSER_
#define _PARSER_

#include <platform.hpp>

#define BUFF_SIZE 8

#define CHANNEL_MASK 0b11000000
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
  DISABLE_MOTORS, // 0x01
  ENBALE_MOTORS, // 0x02
  MOVE_STEPS, // 0x03
  MOVE_TO_ANGLE, //0 x04
  MOVE_CONT, // 0x05
  HOME, // 0x06
  LASER_ON, // 0x07
  LASER_OFF, // 0x08
  SHOOT_L,  // 0x09
  SHOOT_R, // 0x0A
  LOCK_AMMO_R, // 0x0C
  LOAD_AMMO_L, // 0x0D
  LOAD_AMMO_R  // 0x0E
};

namespace PARSER {
  bool rdy = false;  
  // volatile char buff[BUFF_SIZE];  

  void parse(char buff[BUFF_SIZE]) {
    byte axes = 0;
    int steps = 0;
    byte speed = 0;
    bool dir = true;
    int angle = 0;
    
    Serial.println(buff[0], HEX);

    switch (buff[0]) {
      case CMDS::MOVE_STEPS: //Move motor by steps
        Serial.println("Advancing steps");
        axes = CHANNEL_MASK & buff[1] >> 6;
        steps = buff[2] << 8 + buff[3];
        speed = MOVE_SPEED_MASK & buff[1];
        dir = DIR_MASK & buff[1] >> 5;

        PLATFORM::move(axes, dir, speed, steps);
      break;
      
      case CMDS::HOME:
        // start motor homing process payload byte masks
        // 0b11000000 => channels to home [0b00 -> Yaw, 0b01 -> pitch, 0b10 -> camera, 0b11-> all]
        // 0b00111111 => speed as 1/64 of MAX_SPD
        Serial.println("Homing");
        axes = CHANNEL_MASK & buff[1] >> 6;
        speed = HOME_SPEED_MASK & buff[1];

        PLATFORM::home(axes, speed);
      break;
      
      case CMDS::MOVE_CONT: // Continue motor movement by altering their current speed        
        Serial.println("Accelerating");
        axes = CHANNEL_MASK & buff[1] >> 6;      
        speed = MOVE_SPEED_MASK & buff[1];
        dir = DIR_MASK & buff[1] >> 5;

        PLATFORM::setSpeed(axes, dir, speed);      
      break;

      case CMDS::MOVE_TO_ANGLE: // Radial move to an specific angle       
        Serial.println("move to angle") ;
        axes = CHANNEL_MASK & buff[1] >> 6;      
        speed = MOVE_SPEED_MASK & buff[1];
        dir = DIR_MASK & buff[1] >> 5;
        angle = buff[2] << 8 + buff[3];      
        
        PLATFORM::setAngle(axes, dir, speed, angle);
      break;
      
      case CMDS::DISABLE_MOTORS: //disable steppers, reenable with any other command
        Serial.println("Disabling motors");
        PLATFORM::disableMotors();        
      break;

      case CMDS::ENBALE_MOTORS: //enable steppers,
        Serial.println("Enabling motors");
        PLATFORM::enableMotors();      
      break;
      
      case CMDS::SET_LASER: //toggle laser
        Serial.println("Laser");
        PLATFORM::setLaser(buff[1] & 1);        
      break;   

      case CMDS::SHOOT: 
        Serial.println("Shooting Left gun");
        
        Serial.println("Shooting right gun");
        
        // Platform::shoot(0, )
      break;

    }
  }
}

#endif