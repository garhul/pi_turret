# Turret hw control interface

## i2c api notes


### Device address: 0x045

### payload taxonomy:
``` 
typedef struct payload {
  uint8_t cmd: 4;
  uint8_t channel: 4; //corresponds to which motor 
}```

### commands
- Set motor enable/disable state
;


### command list
  - NONE, // 0x00
  - DISABLE_MOTORS, // 0x01
  - ENBALE_MOTORS, // 0x02
  - MOVE_STEPS, // 0x03
  - MOVE_TO_ANGLE, //0 x04
  - MOVE_CONT, // 0x05
  - HOME, // 0x06
  - SET_LASER, // 0x07  
  - LOAD_AMMO, // 0x08
  - LOCK_AMMO, // 0x09
  - SHOOT,  // 0x0A
  - STOP, //0x0B
  - SERVO_PWM, //0x0C
  
  - SET_MOTOR_SPEED
  - SET_MOTOR_ACCEL
  
  - SET_MOTOR_SOFT_LIMIT
  - SET_MOTOR_HARD_LIMIT
  
  - ARM, // 0x08
  - DISARM