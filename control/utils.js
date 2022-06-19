const rpio = require('rpio');

// const MCP9808_ADDR = 0x18;
// const TEMP_REG = 0x05;
// const VL = 0x2c;
const PLATFORM_ADDR = 0x45;


const Bus = {
  init: (baudrate) => {
    rpio.i2cBegin();
    rpio.i2cSetBaudRate(10000);
  },

  write: (addr, buff) => {    
    console.log(`writing to addr:[${addr.toString(16)}] buff: ${buff.toString(16)}`);
    rpio.i2cSetSlaveAddress(addr);    
    rpio.i2cWrite(buff);
  },

  read: (addr, length) => {
    const buff = new Uint8Array(length);
    rpio.i2cSetSlaveAddress(addr);    
    
    const status = rpio.i2cRead(buff);
    console.log(`Read with status [${status}] from [${addr,toString(16)}] [${buff.length}] bytes: [${buff.toString(16)}]`)
    return {status, buff};
  },

  close: () => { rpio.i2cEnd() },
};


Bus.init();

const States = {
  idle: 0,
  busy: 1,  
}

/**
 enum PROPERTIES {
    ACCELERATION,
    DECELERATION,  
    SOFT_LIMIT_POSITION,
    HARD_LIMIT,
    CURRENT_POSITION,
 }


 enum CMDS {
    NONE, // 0x00
    // MOTOR PROPERTIES
    SET_PROPERTY,         //0x01  
    // MOTOR MOVEMENT CONTROL
    DISABLE_MOTORS,       //0x02
    ENBALE_MOTORS,        //0x03
    MOVE_STEPS,           //0x04
    MOVE_TO_ANGLE,        //0x05
    MOVE_UNTIL_LIMIT,     //0x06
    SET_TARGET_SPEED,     //0x07
    STOP,                 //0x08
    HOME,                 //0x09
    
    // GUNS RELATED CONTROL
    SET_LASER_STATE,      //0x0A
    BARREL_NEXT_DART,     //0x0B
    ENABLE_SOLENOID_MS,   //0x0C
    SET_SERVO_POSITION,   //0x0D
  };
 */
const CMDS = {    
  SET_PROPERTY:0x01,
  
  // MOTOR MOVEMENT CONTROL
  DISABLE_MOTORS:0x02,       //0x02
  ENBALE_MOTORS:0x03,        //0x03
  MOVE_STEPS:0x04,           //0x04
  MOVE_TO_ANGLE:0x05,        //0x05
  MOVE_UNTIL_LIMIT:0x06,     //0x06
  SET_TARGET_SPEED:0x07,     //0x07
  STOP:0x08,                 //0x08
  HOME: 0x09,                //0x09
  
  // GUNS RELATED CONTROL
  SET_LASER_STATE: 0x0A,
  BARREL_NEXT_DART: 0x0B,
  ENABLE_SOLENOID_MS: 0x0C,
  SET_SERVO_POSITION: 0x0D,
}

const AXES = {
  yaw: 0x00,
  pitch: 0x01,
  cam: 0x00
}

function bytesToInt(msb, lsb) {
  return (msb << 8) + lsb;
}

class Platform {

  static CMDS = CMDS;
  static AXES = AXES;

  constructor() {
    this.addr = PLATFORM_ADDR;
    this.state = {
      yawPos:0,
      yawStepsPerRev: 0,
      pitchPos:0,
      pitchStepsPerRev: 0,
      camPos:0,
      camStepsPerRev:0,
      vBat: 0,
      laser: 0,
    };

    this.updateInterval = 200;
    this._updateState();
  }

  async CMD(cmd, chan, payload) {
    const cmdByte = (cmd << 2) + chan;
    const buff = new Uint8Array([cmdByte, ...payload]);
    return Bus.write(this.addr, buff);
  }
  
  async setAxisProperty(axis, propKey, value) {
    switch(propKey)   {
      case ACCELERATION:
        this.CMD(SET_PROPERTY, axis, Uint8Array([ACCELERATION, (value >> 8) & 0xFF, value & 0xFF]));
      break;            
     
      case DECELERATION:
        this.CMD(SET_PROPERTY, axis, Uint8Array([DECELERATION, (value >> 8) & 0xFF, value & 0xFF]));
      break;            
     
      case SOFT_LIMIT_POSITION:
        this.CMD(SET_PROPERTY, axis, Uint8Array([SOFT_LIMIT_POSITION, (value >> 8) & 0xFF, value & 0xFF]));
      break;            
     
      case HARD_LIMIT:
        this.CMD(SET_PROPERTY, axis, Uint8Array([HARD_LIMIT, (value >> 8) & 0xFF, value & 0xFF]));
      break;            
     
      case CURRENT_POSITION:
        this.CMD(SET_PROPERTY, axis, Uint8Array([CURRENT_POSITION, (value >> 8) & 0xFF, value & 0xFF]));
      break;            
    }    
  }

  async setAxisStepsPerRev(axis, stepsPerRev) {

  }

  async setAxisAcceleration(axis, accel) {

  }

  async setAxisSoftLimitPos(axis, position) {

  }

  async setAxisHardLimit(axis, hardLimit) {

  }

  async setAxisPosition(axis, pos) {

  }

  async setAxisTargetSpeed(axis, speed) {
    const dir = (speed > 0) ? 1 : 0;
    
    const payload = new Uint8Array(
    [
      (speed >> 8) & 0xFF,
      speed & 0xFF,
      dir &0xFF,
      0x00 //no distance
    ]);
    this.CMD(CMDS.SET_TARGET_SPEED, axis, payload);
  }

  async moveAxis(axis, speed, distance) {
    const dir = (speed > 0) ? 0 : 1;
    
    const payload = new Uint8Array(
      [
        (speed >> 8) & 0xFF,
        speed & 0xFF,
        dir &0xFF,
        (distance >> 8) & 0xFF,
        distance & 0xFF //no distance
      ]);

    this.CMD(CMDS.SET_TARGET_SPEED, axis, payload);
  }

  async home(axis = false) {

  }

  async stop(axis) {
    this.CMD(CMDS.STOP, axis, new Uint8Array([0x00,0x00]));
  }

  
  getState() {
    return this.state;
  }

  async _updateState() {    
    const {status, buff} = await Bus.read(0x45, 16);
    
    if (status !== 0) {
      console.log(`error updating status ${status}`);
      return;
    }
        
    this.state = {
      yawPos: bytesToInt(buff[0],buff[1]),
      yawStepsPerRev: bytesToInt(buff[2],buff[3]),
      pitchPos: bytesToInt(buff[4],buff[5]),
      pitchStepsPerRev: bytesToInt(buff[6],buff[7]),
      camPos:bytesToInt(buff[8],buff[9]),
      camStepsPerRev:bytesToInt(buff[10],buff[11]),
      vBat: bytesToInt(buff[12],buff[13]),
      laser: buff[14],
    };

    setTimeout(() => { this._updateState(); }, this.updateInterval)
  }

}


module.exports = {
  Platform,
  platform: new Platform(),
  Bus,  
};