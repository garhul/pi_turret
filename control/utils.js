const rpio = require('rpio');
const NanoTimer = require('nanotimer');
rpio.init();

const PINS = {
 
};

exports.delay = function delay (ms) {
  return new Promise(r => setTimeout(r, ms));
}


class Motion {  
  constructor() {
    this.pins = {
      A_DIR: 11,
      B_DIR: 7,
      A_STEP: 13,
      B_STEP: 15,
      ENABLE: 22,
    }
    
    this.timer = new NanoTimer();    
    
    this.steps_a = 0;
    this.steps_b = 0;
    this.position_a = 0;
    this.position_b = 0;
    this.pinval_a = 0;
    this.pinval_b = 0;

    this.turning = false;
    this.spd = 0;
    this.baseSpd = 500; //microseconds of delay

    Object.keys(this.pins).forEach(p => {
      rpio.mode(this.pins[p], rpio.OUTPUT);
    });
    
    this.disable();
  }
  
  _updateB() {
    if (this.spd_b === 0) return;
    
    rpio.write(this.pins.B_DIR, (this.spd_b > 0)? 1 : 0);
    this.pinval_b = ++this.pinval_b % 2;
    
    rpio.write(this.pins.B_STEP, this.pinval_b);


    this.timer.setTimeout(()=> this._updateB(),'', `${this.timing_b}u`);
  }

  _updateA() {
    if (this.spd_a === 0) return;
    
    rpio.write(this.pins.A_DIR, (this.spd_a > 0)? 1 : 0);
    this.pinval_a = ++this.pinval_a % 2;
    
    rpio.write(this.pins.A_STEP, this.pinval_a);

    this.timer.setTimeout(() => this._updateB(),'', `${this.timing_a}u`);
  }

  enable() {
    rpio.write(this.pins.ENABLE, 0);
  }

  disable() {
    rpio.write(this.pins.ENABLE, 1);
  }

  rotateA(deg) {

  }

  rotateB(deg) {

  }
 
  setASpeed(spd) {        
    this.spd_a = spd;
    if (spd !== 0) {
      this.timing_a = this.baseSpd / (1000 / Math.abs(spd));
      this._updateA();
    }
  }

  setBSpeed(spd) {
    this.spd_b = spd;
    if (spd !== 0) {    
      this.timing_b = this.baseSpd / (1000 / Math.abs(spd));
      this._updateB();
    }
  }
}


class Guns {
  constructor() {
    this.pins = {
      LASER: 23,
      SOLENOID: 0,
      SERVO: 0,
      BARREL_MOTOR:0,
      BARREL_SENSOR:0,
    }


  }

  advance() {}
  shoot(){}
  
  toggleLaser() {

  }
}

exports.Motion = new Motion();
exports.Guns = new Guns();
