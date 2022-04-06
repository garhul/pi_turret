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

let prevState = {
  X:0,
  Y:0,
  LT:0,
  RT:0,
  RB:0,
  LB:0, 
}

setInterval(()=> {
  var gamepads = navigator.getGamepads ? navigator.getGamepads() : (navigator.webkitGetGamepads ? navigator.webkitGetGamepads() : []);
  if (!gamepads[0]) {
    return;
  }
  const gp = gamepads[0];  
  const st = {
    X:  Math.floor(gp.axes[0] * 1000),
    Y:  Math.floor(gp.axes[1] * 1000),
    LT: Math.floor(gp.buttons[6].value * 1000),
    RT: Math.floor(gp.buttons[7].value * 1000),
    RB: gp.buttons[8].value,
    LB: Math.floor(gp.buttons[9].value * 1000),
    R3: gp.buttons.
    L3:
    X:
    Y:
    A:
    B:
  };

  if(!Object.keys(st).every(v => st[v] === prevState[v])){
    BUS.emit('JOY', st);
    prevState = st;
  }

}, 50); //poll every 50 ms



