const utils = require('./utils');

const delay = async (ms) => new Promise(resolve => setTimeout(resolve, ms));

async function test_ramp_up(motor) {
  for (var i = 1; i < 4; i++) {
    const spd = i * 4
    console.log(`setting speed to ${spd}`)
    await delay(2000);
    motor.accelerate(1, spd);
  }
}

async function test_ramp_down(motor) {
  for (var i = 4; i > 0; i--) {
    const spd = i * 4
    console.log(`setting speed to ${spd}`)
    await delay(2000);
    motor.accelerate(1, spd);
  }
}


(async function() {
  console.log('Ramp up test')
  await test_ramp_up(utils.Yaw);
  await test_ramp_up(utils.Pitch);
  console.log('Ramp down test');
  await test_ramp_down(utils.Yaw);
  await test_ramp_down(utils.Pitch);
})();