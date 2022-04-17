#include <Arduino.h>
#include <Wire.h>

#include <parser.hpp>


#define DEBUG true
#define I2C_ADDR 0x55;

//******************************* BEGIN PIN CONFIG ******************************//
// COMM
#define I2C_SDA PB9 // pin b9
#define I2C_SCL PB8 // pin b8

#define RX PA10 
#define TX PA9 

//******************************* END PIN CONFIG ******************************//



void i2c_req() {
  
}


void setup() {
  
  Wire.begin(3);                
  #ifdef DEBUG 
    Serial1.begin(115200);
  #endif
  

  
  // Wire.onRequest(i2c_req); // read request from master
  Wire.onReceive(PARSER::parse); // write from master

}


void loop() { 
  if (cmd_received) {
    PARSER::parse(cmd);
    cmd_received = false;
  }
  
  // _updateMotors();

  delay(500);
}