#include <Arduino.h>
#include <Wire.h>
#include <platform.hpp>
#include <cmdDispatcher.hpp>
#include <types.h>

#define I2C_ADDR 0x45

//******************************* BEGIN PIN CONFIG ******************************//
// COMM
#define SDA PB9 // pin b9
#define SCL PB8 // pin b8

#define RX PA10 
#define TX PA9 

//******************************* END PIN CONFIG ******************************//
#define BUFF_SIZE 8

inline command parseMessage (uint8_t buff[BUFF_SIZE]) {
  command c = {
    .cmd = static_cast<uint8_t>(buff[0] >> 2),
    .channel = static_cast<uint8_t>(buff[0]>> 6),
    .payload = {buff[1],buff[2],buff[3],buff[4]}
  };
  
  return c;
}


void fn(int len) {
 digitalWrite(PC13, !digitalRead(PC13)); 
 
 uint8_t buff[BUFF_SIZE];
  for (int i = 0; i < len; i++) {
    buff[i] = Wire.read();
  }
  
  COMMAND_DISPATCHER::dispatch(parseMessage(buff));  
}

void i2c_req() {
  Serial.println("un req");
  Wire.write(PLATFORM::getState());
  digitalWrite(PC13, !digitalRead(PC13)); 
} 

void setup() {  
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13,HIGH);
  PLATFORM::begin();  
  Wire.setSDA(SDA);
  Wire.setSCL(SCL);
  Wire.begin(I2C_ADDR);
  Serial.begin(115200);  
  Wire.onReceive(fn); // write from master
  Wire.onRequest(i2c_req);  
  delay(5000);
  Serial.println("RDY");
}

void loop() { 
  PLATFORM::update();  
}
