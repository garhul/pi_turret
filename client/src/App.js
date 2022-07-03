import './App.css';
import {GamepadView, VideoFeed, RadarView, PlatformStatus } from './Controls';
import React from 'react';

function App() {
  return (
    <div className="App">
      <VideoFeed></VideoFeed>
      <div style={{"display":"flex-column"}}>
        <GamepadView></GamepadView>
        <PlatformStatus></PlatformStatus>
        <RadarView></RadarView>
      </div>
    </div>
  );
}

export default App;
