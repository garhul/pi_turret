const { monitorEventLoopDelay } = require('perf_hooks');
const { WebSocketServer } = require('ws');
const { Motion } = require('./utils');
const NanoTimer = require('nanotimer');

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
      Motion.setASpeed(msg.Ax);
      Motion.setBSpeed(msg.Ay);
    }
  });

  ws.send(JSON.stringify({ev:'SV',msg:'ok'}));
});

