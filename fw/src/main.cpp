#include <Arduino.h>
#include <Wire.h>
#include <platform.hpp>
#include <cmdDispatcher.hpp>
#include <types.h>

#define I2C_ADDR 0x45

//******************************* BEGIN PIN CONFIG ******************************//
// COMM
#define SDA PB7 // PB9 
#define SCL PB6 // PB8 

// #define RX PA10 
// #define TX PA9 

//******************************* END PIN CONFIG ******************************//
#define BUFF_SIZE 9

inline command parseMessage (uint8_t buff[BUFF_SIZE]) {
  command c = {
    .cmd = static_cast<uint8_t>(buff[0] >> 2),
    .channel = static_cast<uint8_t>(buff[0] & 0x03),
    .payload = {buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8]}
  };
  
  return c;
}



void fn(int len) {
  digitalWrite(PC13, !digitalRead(PC13)); 
  Serial.print("[");

  uint8_t buff[BUFF_SIZE];
  for (int i = 0; i < len; i++) {
    buff[i] = Wire.read();
    Serial.print(buff[i],HEX);
    Serial.print(" ");
  }  
  Serial.println("]");

  COMMAND_DISPATCHER::dispatch(parseMessage(buff));  
}

void i2c_req() {
  // Serial.println("un req");  
  const uint8_t * buffer = PLATFORM::getState();
  Serial.print("[");
  for (int i= 0; i< 16; i++) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("]");

  Wire.write(buffer, 16);
  digitalWrite(PC13, !digitalRead(PC13)); 
} 

void setup() {  
  Serial.begin(115200);  
  Serial.println("READY");
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13,HIGH);
  PLATFORM::begin();  
  Wire.setSDA(SDA);
  Wire.setSCL(SCL);
  Wire.begin(I2C_ADDR);
  Wire.onReceive(fn); // write from master
  Wire.onRequest(i2c_req);  
  delay(5000);
  Serial.println("RDY");
}

void loop() { 
  PLATFORM::update();  
}
