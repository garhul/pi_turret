const rpio = require('rpio');

const MCP9808_ADDR = 0x18;
const TEMP_REG = 0x05;
const VL = 0x2c;
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

//read state of platform
const getPlatformData = async () => {
  const b = await getBus();
  const buff = Uint8Array(16);
  const d = await b.i2cReadSync(PLATFORM_ADDR, buff.length, buff);
  console.log(`data: ${d}`);
  return d;
}


const States = {
  idle: 0,
  busy: 1,  
}


class Axis {
  constructor(index, steps, speed) {
    this.addr = PLATFORM_ADDR;
    this.state = States.idle;
    this.index = index;
    this.steps_per_rev = steps;
    this.speed = speed;    
  }

  async accelerate(dir, speed) {
    if (speed < 0 || speed > 31) 
      throw new Error (`Speed value [${speed}] out of range`);
        
    const cmd = 0x05;
    const chan = this.index;
    const data = (chan << 6) + (dir << 5) + speed;
    
    const buff = new Uint8Array([cmd,data]);    
    return Bus.write(this.addr, buff);    
  }

  async stop() {
    const cmd = 0x0B;
    const chan = this.index;
    const data = (chan << 6);
    Bus.write(this.addr, new Uint8Array([cmd, data]));
  }


  advanceSteps(steps, dir, speed) {

  };

  moveToAngle() {

  }

  home() {

  }

}


class Gun {
  constructor() {
    this.state = 'idle';
  }

  lock(){};
  load(){};

}



module.exports = {
  Yaw: new Axis(0x00, 200, 200),
  Pitch: new Axis(0x01, 200, 200),
  Guns: [new Gun(), new Gun()],
  Bus: Bus,  
};