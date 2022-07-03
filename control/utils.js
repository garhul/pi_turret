const rpio = require('rpio');

// const MCP9808_ADDR = 0x18;
// const TEMP_REG = 0x05;
// const VL = 0x2c;
const PLATFORM_ADDR = 0x45;

export const Bus = {
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
    console.log(`Read with status [${status}] from [${addr.toString(16)}] [${buff.length}] bytes: [${buff.toString(16)}]`)
    return {status, buff};
  },

  close: () => { rpio.i2cEnd() },
};

Bus.init();



const States = {
  idle: 0,
  busy: 1,  
}

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

export const PROPERTIES = {
  ACCELERATION: 0,
  DECELERATION: 1,  
  SOFT_LIMIT_POSITION: 2,
  HARD_LIMIT: 3,
  CURRENT_POSITION: 4,
}

export const AXES = {
  YAW: 0x00,
  PITCH: 0x01,
  CAM: 0x00
}

function bytesToInt(msb, lsb) {
  return (msb << 8) + lsb;
}
    
const State = {
  yawPos:0,
  yawStepsPerRev: 0,
  pitchPos:0,
  pitchStepsPerRev: 0,
  camPos:0,
  camStepsPerRev:0,
  vBat: 0,
  laser: 0,
};

const updateInterval = 200;
function _buildMovementPayload(speed, distance = 0) {
    const dir = (speed > 0) ? 1 : 0;
    const spd = Math.abs(speed);
    return new Uint8Array(
    [
      (spd >> 8) & 0xFF,
      spd & 0xFF,
      dir &0xFF,
      (distance >> 8) & 0xFF,
      distance & 0xFF //no distance
    ]);
}

async function _updateState() {    
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

async function _cmd(cmd, chan, payload) {
  const cmdByte = (cmd << 2) + chan;
  const buff = new Uint8Array([cmdByte, ...payload]);
  return Bus.write(PLATFORM_ADDR, buff);
}
  
export async function setAxisProperty(axis, propKey, value) {
  if (!(popkey in PROPERTIES)) 
    throw new Error(`SetAxisProperty:: Property ${propKey} out of range`);

  _cmd(CMDS.SET_PROPERTY, axis, Uint8Array([propKey, (value >> 8) & 0xFF, value & 0xFF]));    
}

export async function setAxisTargetSpeed(axis, speed) {  
  if (!(axis in AXES)) throw new Error(`setAxisTargetSpeed:: Axis ${axis} out of range`);

  _cmd(CMDS.SET_TARGET_SPEED, axis, _buildMovementPayload(speed));
}

export async function moveAxis(axis, speed, distance) {
  if (!(axis in AXES)) throw new Error(`moveAxis:: Axis ${axis} out of range`);

  _cmed(CMDS.MOVE_STEPS, axis, _buildMovementPayload(speed, distance));
}

export async function home(axis = false) {
  if (!(axis in AXES)) throw new Error(`home:: Axis ${axis} out of range`);
  _cmd(CMDS.HOME, axis, _buildMovementPayload(speed, distance));
}

export async function stop(axis) {
  _cmd(CMDS.STOP, axis, new Uint8Array([0x00,0x00]));
}

async function delay(ms) {
  return new Promise (resolve => setTimeout((resolve) => ms ));
}

async function _init() {
  // set default properties of the platform
  setAxisProperty()
  await DelayNode()

}

export function initialize () {
  _init();
  _updateState();
}