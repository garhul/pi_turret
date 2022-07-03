#ifndef _TYPES_
#define _TYPES_
  #include <Arduino.h>

  #ifndef BUFF_LEN
    #define BUFF_LEN 8
  #endif

  struct platformState {
    uint16_t yPos;
    uint16_t yStepsRev;
    uint16_t pPos;
    uint16_t pStepsRev;
    uint16_t camPos;
    uint16_t camStepsRev;    
    uint16_t vBat; // vbat in mv
    bool laser; // laser state
  };

  struct MotorProperties {
    uint8_t channel; // 1
    uint16_t acceleration;  // 3
    uint16_t maxSpeed; // 5
    uint16_t stepsPerRev; // 7
    uint16_t softLimitPosition; // 9
    bool hardLimit; // 10
  };

  struct propertyPayload {
    uint8_t propertyId;
    uint16_t value;
  };
  
  struct movementPayload {
    uint16_t speed;
    bool dir;
    uint16_t distance;
  };

  struct command {
    uint8_t cmd: 6;
    uint8_t channel: 2; // 4 channels : 0-> yaw, 1-> pitch, 2-> camera, 3 -> reserved
    uint8_t payload[BUFF_LEN]; // rest of payload data
  };

#endif