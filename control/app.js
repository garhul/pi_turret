const { monitorEventLoopDelay } = require('perf_hooks');
const { WebSocketServer } = require('ws');
const { MotorX, MotorY } = require('./utils');
const NanoTimer = require('nanotimer');

const t = new NanoTimer();
// const MOTOR_B_ACC = 1000;
// const MOTOR_A_ACC = 1000;

// const MOTOR_A_MAX_SPD = 2000;
// const MOTOR_B_MAX_SPD = 2000;

// function udpdate_pos() {
//   console.log('Updating position...');
// }


const wss = new WebSocketServer({ port: 3030 });

console.log('websocket started, awaiting connections...');

MotorX.rotate(500);

wss.on('connection', function connection(ws) {
  console.log('client connected');
  console.log(ws);
  
  ws.on('message', function message(data) {
    console.log('received: %s', data);
    
    if (data.ev === 'PAD') {
      MotorX.rotate(data.x);
      MotorY.rotate(data.y);
    }

  });

  ws.send('something');
});


//test rotation for motorx



// var start = new Date();
// var i = 0;

// t.setInterval(function(){
//     if (++i >= 1000) {
//         var end = new Date();
//         console.log(`The average interval was ${((end-start)/1000)} milliseconds`);
//         t.clearInterval();
//     }
// },'', '100u');

