class Bus {
  constructor(socket) {
    this.socket = new WebSocket(`ws://${window.location.host.split(':')[0]}:3030`);
    this.socket = new WebSocket(`ws://turret.local:3030`);

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

class Pad {
  constructor() {
    this.listeners = {};
    this.connected = false;
    this._st = {
      connected:false,  
      Ax:0,
      Ay:0,
      Bx:0,
      By:0,
      LT:0,
      RT:0,
      RB:0,
      LB:0,
      RSB:0,
      LSB:0,
      X:0,
      Y:0,
      A:0,
      B:0,
      up:0,
      down:0,
      left:0,
      right:0
    };
    
    setInterval(()=> {
      var gamepads = navigator.getGamepads ? navigator.getGamepads() : (navigator.webkitGetGamepads ? navigator.webkitGetGamepads() : []);
      if (!gamepads[0]) {
        this._st.connected = false;
        return;
      }

      const gp = gamepads[0];  
      const st = {
        connected: true,
        Ax:  Math.floor(gp.axes[0] * 1000),
        Ay:  Math.floor(gp.axes[1] * 1000),
        Bx: Math.floor(gp.axes[2] * 1000),
        By: Math.floor(gp.axes[3] * 1000),
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
    
      if(!Object.keys(st).every(v => st[v] === this._st[v])){
        this._ev(st);
        this._st = st;
      }
    
    }, 50);        
  }

  get st() {
    return this._st
  }


  _ev(data) {    
    const ev = 'change';
    if (this.listeners[ev].length === 0) {
      console.warn(`JP No handler registered for ev ${ev}`);
      console.dir(data);
      return;
    }

    this.listeners[ev].forEach(fn => fn.call(this, data));      
  }

  onChange(fn) { 
    const ev = 'change';
    if (!this.listeners[ev]) this.listeners[ev] = [];
    this.listeners[ev].push(fn);
    console.dir(this.listeners);
  }

}

const p = new Pad();
p.onChange((d) => BUS.emit('PAD',d));
export const PAD = p;



