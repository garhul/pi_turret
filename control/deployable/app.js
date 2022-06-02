const { WebSocketServer } = require('ws');
const utils = require('./utils');

const wss = new WebSocketServer({ port: 3030 });

wss.on('listening',()=> {
  console.log('websocket started, awaiting connections...');
});


const clients = [];
wss.on('connection', function connection(ws) {
  console.log('client connected');
  clients.push(ws);
  // console.log(ws);
  
  ws.on('close', () => {
    const i = clients.indexOf(ws);
    clients.splice(i,1);
  });

  ws.on('message', function message(d) {
    const {ev, msg} = JSON.parse(d);
    // console.log(msg, ev);
    if (ev === 'PAD') {

      const yspd = Math.floor((Math.abs(msg.Ax) * 31) / 1000);
      const pspd = Math.floor((Math.abs(msg.Ay) * 31) / 1000);
      const stop =  msg.X;      
      const home = msg.Y;

      if (stop) {
        utils.Yaw.stop();
        utils.Pitch.stop();
      } else {
        utils.Yaw.accelerate((msg.Ax > 0)? 1 :0, yspd);
        utils.Pitch.accelerate((msg.Ay > 0)? 1 :0, pspd);
      }
    }
    
  });

  ws.send(JSON.stringify({ev:'SV',msg:'ok'}));
});
