import './App.css';
import {GamepadView, VideoFeed, RadarView } from './Controls';

function App() {
  return (
    <div className="App">
      <VideoFeed></VideoFeed>
      <div style={{"display":"flex-column"}}>
        <GamepadView></GamepadView>
        <RadarView></RadarView>
      </div>
    </div>
  );
}

export default App;
