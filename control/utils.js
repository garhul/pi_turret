const rpio = require('rpio');
const NanoTimer = require('nanotimer');
rpio.init();

const PINS = {
  A_DIR: 11,
  B_DIR: 7,
  A_STEP: 13,
  B_STEP: 15,
  ENABLE: 22,
};

exports.delay = function delay (ms) {
  return new Promise(r => setTimeout(r, ms));
}

/* 
let's say max speed is 60 rpm, so 1 rev per second, so steps_per_rev per second.

so, if I'm at maximum speed I'd expect to create steps_per_rev pulses in 1 second, 

so 1000ms / steps_per_rev -> 1000 / 200 = 5 (1 step per ms)




*/

class Motor {  
  constructor(step_pin, dir_pin) {
    this.step_pin = step_pin;
    this.dir_pin = dir_pin;    
    this.steps = 0;
    this.steps_per_rev = 200;
    this.position = 0;
    this.turning = false;
    this.spd = 0;
    this.timer = new NanoTimer();
    this.pinval = 0;

    rpio.mode(dir_pin, rpio.OUTPUT);
    rpio.mode(step_pin, rpio.OUTPUT);
    
    rpio.write(this.step_pin, 0);
    rpio.write(this.dir_pin, 0);
    
    this._update();
    this.timer.setInterval(()=> {this._update()},'', '500u')
  }
  
  _update() {
    if (this.spd === 0) return;
    this.pinval = ++this.pinval % 2;
    
    rpio.write(this.step_pin, this.pinval);
  }

  rotate(spd) {    
    console.log('rotation set');
    rpio.write(this.dir_pin, (spd > 0)? 1 : 0);
    this.spd = spd;    
    console.log(this.spd);
  }
}


exports.MotorX = new Motor(PINS.A_STEP,PINS.A_DIR);
// exports.MotorY = new Motor(PINS.B_STEP,PINS.B_DIR);

exports.GunR = {}
exports.GunL = {}
