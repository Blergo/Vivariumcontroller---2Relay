#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SimpleModbusSlaveSoftwareSerial.h>
#include <EEPROM.h>

#define Rly0Pin 3
#define Rly1Pin 4

#define RX            1
#define TX            0
#define RS485_EN      -1
#define BAUD_RATE     9600
unsigned int deviceID = 1;
unsigned int NewID;

unsigned long u32wait = millis() + 500;

enum  {
  SET_ID,
  ABILITY,
  ERR_CODE,
  TOTAL_ERRORS,
  RELAY0,
  RELAY1,
  TOTAL_REGS_SIZE 
};

uint16_t holdingRegs[TOTAL_REGS_SIZE];

SoftwareSerial mySerial(RX, TX);

void setup() {
  pinMode(Rly0Pin, OUTPUT);
  digitalWrite(Rly0Pin, HIGH);
  pinMode(Rly1Pin, OUTPUT);
  digitalWrite(Rly1Pin, HIGH);
  mySerial.begin(BAUD_RATE);
  holdingRegs[1] = 82;
  EEPROM.begin();
  EEPROM.get(1, NewID);
  if (NewID > 255){
    EEPROM.put(1, deviceID);
  }
  if (NewID > 1 && NewID != deviceID){
    deviceID = NewID;
  }
  holdingRegs[0] = deviceID;
  modbus_configure(&mySerial, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);
}

void reboot(void) {
  noInterrupts();
  CLKPR = 0b10000000;
  CLKPR = 0;
  void (*ptrToFunction)();
  ptrToFunction = 0x0000;
  (*ptrToFunction)();
}

void loop() {
  if(millis() > u32wait){
    if(holdingRegs[4] == 1){
      digitalWrite(Rly0Pin, LOW);
    }
    else if(holdingRegs[4] == 0){
      digitalWrite(Rly0Pin, HIGH);
    }
    if(holdingRegs[5] == 1){
      digitalWrite(Rly1Pin, LOW);
    }
    else if(holdingRegs[5] == 0){
      digitalWrite(Rly1Pin, HIGH);
    }
    u32wait = millis() + 500;
  }
  
  if(holdingRegs[0] != deviceID){
    EEPROM.write(1, holdingRegs[0]);
    delay(100);
    reboot();
  }
    
  holdingRegs[3] = modbus_update(holdingRegs);
}