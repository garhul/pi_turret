import { getFCP } from "web-vitals";

class Bus {
  constructor(socket) {
    this.socket = new WebSocket(`ws://${window.location.host.split(':')[0]}:3030`);;
    this.listeners = [];
    this.of = [];
    this.client_identity = Math.random().toString(36).replace(/[^a-z0-9]+/g, '').substr(0, 18);
    
    this.socket.addEventListener('open', (event) => {
      console.info('Socket connected')
      this.emit('CLI', { id: this.client_identity }); //so the server can identify this client from now on
      this.handleEvent('open');
    });
    
    this.socket.addEventListener('message', (event) => {      
      const {ev, data} = JSON.parse(event.data);
      this.handleEvent(ev, data);
    });

    this.socket.addEventListener('error', (err) => {
      console.error(err);
      this.handleEvent('error');
    });

    this.socket.addEventListener('close', () => {
      console.log('socket closed');
      this.handleEvent('close');
    })
  }

  emit(ev, msg) {    
    if (this.socket.readyState === 1) {      
      this.socket.send(JSON.stringify({ev, msg}));    
    } else {
      console.warn('Socket not ready to send', ev, msg);
    }
  }

  handleEvent(ev, data) {
    let hndlrs = 0;
    this.listeners.forEach(val => {      
      if (val.ev === ev) {
        hndlrs++;
        val.fn.call(this, data);
      }
    });

    if (hndlrs === 0) {
      console.warn(`No handler registered for ev ${ev}`);
      console.dir(data);
    }
  }

  on(ev, fn) {
    this.listeners.push({ev, fn});
  }
}

export const BUS = new Bus();


//now for the joystick code

let prevState = {};

setInterval(()=> {
  var gamepads = navigator.getGamepads ? navigator.getGamepads() : (navigator.webkitGetGamepads ? navigator.webkitGetGamepads() : []);
  if (!gamepads[0]) {
    return;
  }
  const gp = gamepads[0];  
  const st = {
    Ax:  Math.floor(gp.axes[0] * 1000),
    Ay:  Math.floor(gp.axes[1] * 1000),
    LT: Math.floor(gp.buttons[6].value * 1000),
    RT: Math.floor(gp.buttons[7].value * 1000),
    RB: gp.buttons[5].value,
    LB: gp.buttons[4].value,
    RSB: gp.buttons[11].value,
    LSB: gp.buttons[10].value,
    X: gp.buttons[2].value,
    Y: gp.buttons[3].value,
    A: gp.buttons[0].value,
    B: gp.buttons[1].value,
    up: gp.buttons[12].value,
    down: gp.buttons[13].value,
    left: gp.buttons[14].value,
    right: gp.buttons[15].value
  };

  if(!Object.keys(st).every(v => st[v] === prevState[v])){
    BUS.emit('JOY', st);
    prevState = st;
  }

}, 50); //poll every 50 ms



