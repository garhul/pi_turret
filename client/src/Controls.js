import { useEffect, useState } from "react";
import { BUS, PAD } from "./io";

export function GamepadView() {
  let [pad, updatePad] = useState(PAD.st);
  
  useEffect(() => {           
    PAD.onChange(d => {      
      updatePad(d);
    });    
  }, []);

  return (        
    <div id="GamePadView">
      <div className="controlHeader">
        <h3>JoyPad state: <span>{(pad.connected) ? 'on':'off' }</span></h3>      
      </div>
      <div className="controlBody">
        <div>
          <svg viewBox="-2.2 -2.2 4.4 4.4" width="240" height="240">
            <circle cx="0" cy="0" r="2" fill="none" stroke="#444" strokeWidth="0.04" />
            <path d="M0,-2L0,2M-2,0L2,0" stroke="#444" strokeWidth="0.02" />
            <circle cx={(pad) ? pad.Ax / 500 : 0} cy={(pad) ? pad.Ay / 500 : 0} r="0.11" fill="#888" className="axis" />        
          </svg>
        </div>
        <div style={{"display":"flex"}}>
          <div >
            <div className={(pad.RB) ? 'btnStateOn':'btnStateOff'}>RB</div>
            <div className={(pad.LB) ? 'btnStateOn':'btnStateOff'}>LB</div>
            <div className={(pad.RT) ? 'btnStateOn':'btnStateOff'}
              style={{"background": `linear-gradient(90deg, #666 ${pad.RT/10}%, #000 ${pad.RT/10}%)`}}>RT</div> {/*range*/}
            <div className={(pad.LT) ? 'btnStateOn':'btnStateOff'}
              style={{"background": `linear-gradient(90deg, #666 ${pad.LT/10}%, #000 ${pad.LT/10}%)`}}>LT</div> {/*range*/}
            {/* <div className={(pad.RSB) ? 'btnStateOn':'btnStateOff'}>RSB</div>
            <div className={(pad.LSB) ? 'btnStateOn':'btnStateOff'}>LSB</div> */}
          </div>
          <div >
            <div className={(pad.A) ? 'btnStateOn':'btnStateOff'}>A</div>
            <div className={(pad.B) ? 'btnStateOn':'btnStateOff'}>B</div>
            <div className={(pad.X) ? 'btnStateOn':'btnStateOff'}>X</div>
            <div className={(pad.Y) ? 'btnStateOn':'btnStateOff'}>Y</div>
            {/* <div className={(pad.RSB) ? 'btnStateOn':'btnStateOff'}>RSB</div>
            <div className={(pad.LSB) ? 'btnStateOn':'btnStateOff'}>LSB</div> */}
          </div>          
        </div>
      </div>
    </div>
  );
}

export function VideoFeed() {
  return (
    <div id="VideoFeed">
      Video goes here
    </div>
  )
}

export function RadarView() {


  return (
    <div id="RadarView">
      <div className="controlHeader">
        <h3>Radar</h3>
      </div>
      <div className="controlBody">
        <svg viewBox="-2.2 -2.2 4.4 4.4" width="400" height="400">
          <circle cx="0" cy="0" r="2" fill="#131" stroke="#121" strokeWidth="0.04" />
          <path d="M0,-2L0,2M-2,0L2,0" stroke="#121" strokeWidth="0.02" />    
          <circle cx="0.1" cy="1" r="0.05" fill="#080" className="target" />        
          <circle cx="-0.2" cy="-0.5" r="0.05" fill="#080" className="target" />        
        </svg>
      </div>
    </div>
  )
}