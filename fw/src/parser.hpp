#include <Wire.h>
#include <motors.hpp>

#define BUFF_SIZE 16

#define CHANNEL_MASK 0b11000000
#define HOME_SPEED_MASK 0b00111111
#define MOVE_SPEED_MASK 0b00011111
#define DIR_MASK 0b00100000



//******************************* I2C Codes samples **********************************//
//Movement commands
/*

  1st byte    2nd byte   3rd byte
  [--------] [--------] [--------]
  
  0x0A 0x0F


  0x0A Homing


  Command
    H: Home motor 

    D: Disable mottor


 
 / Base Command | Axis | units | direction | [spd]
// H/M/A/R/D      P/Y/C    0000    +/-        0000

  0b111 1    11   11 
    cmd dir  axis payload 


*/



// H -> Homing move (? for all axes), spd is in units, direction is ignored
// M -> Linear move, units represent amount of steps, spd is used here for speed of movement
// A -> Acceleration move, units represent speed
// R -> Radial move, move by *units* degrees, at spd speed
// D (disable motors)



// Gun commands
// Base Command | value | [extra_val]
// S/L/


// SL100 (Shoot Left gun (open solenoid) for 100 ms )
// SR50  (shoot right gun (open solenoid for 50 ms))
// L1  (Laser on, L0 Laser off)
// reload gun? 

// Extra commands

//? V Requests info from sensors (current, voltage, pressure)
//? M Requests motor position information

//? L Requests Lidar info ? ?  ? ?

//$A => set acceleration for motors ?


namespace PARSER {
  bool rdy = false;
  bool _laser = false;
  volatile char buff[BUFF_SIZE];
  // char [] command_queue;

  void parse(int len) {
 
    for (int i = 0; i < len; i++) {
      buff[i] = Wire.read();
    }

    #ifdef DEBUG
      Serial.print("parsing command: ");
    for (int i =0; i < 32; i++) {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println(" |");
    #endif

  switch (buff[0]) {
    case 'M': //Move motor by steps
      
      byte axes = CHANNEL_MASK & buff[1] >> 6;
      int steps = buff[2] << 8 + buff[3];
      byte speed = MOVE_SPEED_MASK & buff[1];
      boolean dir = DIR_MASK & buff[1] >> 5;

      MOTORS::move(axes, dir, speed, steps);
    break;
    
    case 'H':
      // start motor homing process payload byte masks
      // 0b11000000 => channels to home [0b00 -> Yaw, 0b01 -> pitch, 0b10 -> camera, 0b11-> all]
      // 0b00111111 => speed as 1/64 of MAX_SPD

      byte axes = CHANNEL_MASK & buff[1] >> 6;
      byte speed = HOME_SPEED_MASK & buff[1];

      MOTORS::home(axes, speed);
    break;
    
    case 'A':
      // Move motors by alter their current speed        
      byte axes = CHANNEL_MASK & buff[1] >> 6;      
      byte speed = MOVE_SPEED_MASK & buff[1];
      boolean dir = DIR_MASK & buff[1] >> 5;

      MOTORS::setSpeed(axes, dir, speed);      
    break;
    case 'R':
      byte axes = CHANNEL_MASK & buff[1] >> 6;      
      byte speed = MOVE_SPEED_MASK & buff[1];
      boolean dir = DIR_MASK & buff[1] >> 5;
      int angle = buff[2] << 8 + buff[3];      
      
      MOTORS::setAngle(axes, dir, speed, angle);
    break;
    
    case 'D': //disable steppers, reenable with any other command
      digitalWrite(MOTOR_ENABLE, HIGH);
    break;
    
    case 'L': //toggle laser
      _laser = !_laser;
      digitalWrite(LASER, _laser);
    break;   
  }

}

}