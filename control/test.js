const {Platform, platform} = require('./utils');

const delay = async (ms) => new Promise(resolve => setTimeout(resolve, ms));

async function test_ramp_up(motor) {
  for (var i = 21; i < 31 ; i++) {
    const spd = i;
    console.log(`setting speed to ${spd}`)
    await delay(500);
    motor.accelerate(0, spd);
	await delay(300);
	  //motor.stop();
  }
}

async function test_ramp_down(motor) {
  for (var i = 21; i > 0; i--) {
    const spd = i; 
    console.log(`setting speed to ${spd}`)
    await delay(300);
    motor.accelerate(0, spd);
  }
}


async function test_turning(motor) {
 motor.stop();
 motor.accelerate(0,10);
 await delay(10000);
 motor.accelerate(0,20);
 await delay(3000);
 motor.accelerate(0,31);
 await delay(3000);
 //motor.stop();
 //await delay(100);
 motor.accelerate(1,25);
 await delay(10000);
 motor.stop();
	
};

function bytesToInt(msb, lsb) {
  return (msb << 8) + lsb;
}

async function test_read() {
  const {status, buff} = await utils.Bus.read(0x45, 16);

  console.log(status);
  console.log(buff);
  
  //do some parsing!
  const data = {
    yawPos: bytesToInt(buff[0],buff[1]),
    yawStepsPerRev: bytesToInt(buff[2],buff[3]),
    pitchPos: bytesToInt(buff[4],buff[5]),
    pitchStepsPerRev: bytesToInt(buff[6],buff[7]),
    camPos:bytesToInt(buff[8],buff[9]),
    camStepsPerRev:bytesToInt(buff[10],buff[11]),
    vBat: bytesToInt(buff[12],buff[13]),
    laser: buff[14],
  };

  console.dir(data);
}

(async function() {
  console.log("******** PLATFORM STATE *************");
  console.dir(platform.getState());
  platform.moveAxis(Platform.AXES.yaw, - 2500, 300);
  await delay(2000);
  platform.setAxisTargetSpeed(Platform.AXES.yaw, 2500)
  await delay(2000);
  platform.stop(Platform.AXES.yaw);
  console.dir(platform.getState());


})();
