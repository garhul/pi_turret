#include <Arduino.h>
#include <Wire.h>
#include <platform.hpp>
#include <parser.hpp>

#define I2C_ADDR 0x45

//******************************* BEGIN PIN CONFIG ******************************//
// COMM
#define SDA PB9 // pin b9
#define SCL PB8 // pin b8

#define RX PA10 
#define TX PA9 

//******************************* END PIN CONFIG ******************************//

void fn(int len) {
 digitalWrite(PC13, !digitalRead(PC13)); 
 char buff[8];
  for (int i = 0; i < len; i++) {
    buff[i] = Wire.read();
  }
  
  PARSER::parse(buff);
  
  // Serial.print("parsing command: ");
  //   for (int i =0; i < BUFF_SIZE; i++) {
  //     Serial.print(buff[i], HEX);
  //     Serial.print(" ");
  //   }

  // Serial.println(" |");
  
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
