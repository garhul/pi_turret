# Turret hw control interface

## i2c api notes

### Device address: 0x045

### Payload taxonomy:

``` 
struct payload {
  uint8_t cmd: 6;
  uint8_t channel: 2; 
  uint8_t payload[] 
}
```

## Properties list
|||
|---------------------|---- |
| ACCELERATION        | 0x00 |
| DECELERATION        | 0x01 |
| MAX_SPEED           | 0x02 |
| SOFT_LIMIT_POSITION | 0x03 |
| CURRENT_POSITION    | 0x04 |


## Command list
|||
|-----------------------|------|
|  SET_PROPERTY         | 0x01 |
|  DISABLE_MOTORS       | 0x02 |
|  ENBALE_MOTORS        | 0x03 |
|  MOVE_STEPS           | 0x04 |
|  MOVE_TO_ANGLE        | 0x05 |
|  MOVE_UNTIL_LIMIT     | 0x06 |
|  SET_TARGET_SPEED     | 0x07 |
|  STOP                 | 0x08 |
|  HOME                 | 0x09 |
|  SET_LASER_STATE      | 0x0A |
|  BARREL_NEXT_DART     | 0x0B |
|  ENABLE_SOLENOID_MS   | 0x0C |
|  SET_SERVO_POSITION   | 0x0D |