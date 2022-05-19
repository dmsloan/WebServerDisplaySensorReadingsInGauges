// Get current sensor readings when the page loads  
window.addEventListener('load', getReadings);

// Create Temperature Gauge
var gaugeTemp = new LinearGauge({
  renderTo: 'gauge-temperature',
  width: 120,
  height: 400,
  units: "Temperature C",
  minValue: 0,
  startAngle: 90,
  ticksAngle: 180,
  maxValue: 40,
  colorValueBoxRect: "#049faa",
  colorValueBoxRectEnd: "#049faa",
  colorValueBoxBackground: "#f1fbfc",
  valueDec: 2,
  valueInt: 2,
  majorTicks: [
      "0",
      "5",
      "10",
      "15",
      "20",
      "25",
      "30",
      "35",
      "40"
  ],
  minorTicks: 4,
  strokeTicks: true,
  highlights: [
      {
          "from": 30,
          "to": 40,
          "color": "rgba(200, 50, 50, .75)"
        }
      ],
      colorPlate: "#fff",
      colorBarProgress: "#CC2936",
      colorBarProgressEnd: "#049faa",
  borderShadowWidth: 0,
  borders: false,
  needleType: "arrow",
  needleWidth: 2,
  needleCircleSize: 7,
  needleCircleOuter: true,
  needleCircleInner: false,
  animationDuration: 1500,
  animationRule: "linear",
  barWidth: 10,
}).draw();

// Create Amperage Gauge
var gaugeAmp = new RadialGauge({
  renderTo: 'gauge-amperage',
  width: 300,
  height: 300,
  units: "Amperage",
  minValue: 0,
  maxValue: 100,
  colorValueBoxRect: "transparent",
  colorValueBoxRectEnd: "#049faa",
  colorValueBoxBackground: "#f1fbfc",
  valueInt: 2,
  valueDec: 1,
  valueTextShadow: true,
  majorTicks: [
    "0",
    "20",
    "40",
      "60",
      "80",
      "100"
    ],
    minorTicks: 4,
    strokeTicks: true,
    highlights: [
      {
        "from": 80,
        "to": 100,
        "color": "#03C0C1"
      }
    ],
    colorPlate: "#fff",
    borderShadowWidth: 0,
    borders: false,
    needleType: "line",
    colorNeedle: "#007F80",
    colorNeedleEnd: "#007F80",
    needleWidth: 2,
    needleCircleSize: 3,
    colorNeedleCircleOuter: "#007F80",
    needleCircleOuter: true,
    needleCircleInner: false,
    animationDuration: 1500,
    animationRule: "linear"
  }).draw();

  // Create horizAmperage Gauge
  var gaugeHorizAmp = new LinearGauge({
  renderTo: 'gauge-HorizAmperage',
  width: 300,
  height: 250,
  units: "Amperage",
  minValue: 0,
  maxValue: 100,
  valueInt: 2,
  majorTicks: [
      "0",
      "20",
      "40",
      "60",
      "80",
      "100"
    ],
    minorTicks: 10,
    strokeTicks: true,
    highlights: [
        {
          "from": 80,
          "to": 90,
          "color": "rgba(250, 250, 50, .75)"
        }, 
        {
          "from": 90,
          "to": 100,
          "color": "rgba(200, 50, 50, .75)"
        } 
      ],
  colorPlate: "#fff",
  borderShadowWidth: 0,
  borders: false,
  barBeginCircle: false,
  tickSide: "left",
  numberSide: "left",
  needleSide: "left",
  needleType: "arrow",
  needleWidth: 5,
  colorNeedle: "red",
  colorNeedleEnd: "red",
  needleShadow: false,
  colorBarProgressEnd: "#CC2936",
  animation: true,
  animationDuration: 1500,
  animationRule: "bounce",
  animationTarget: "plate",
  ticksWidth: 15,
  ticksWidthMinor: 10
}).draw();

// Function to get current readings on the webpage when it loads for the first time
function getReadings(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      console.log(myObj);
      var temp = myObj.temperature;
      var amp = myObj.amperage;
      gaugeTemp.value = temp;
      gaugeAmp.value = amp;
      gaugeHorizAmp.value = amp;
    }
  }; 
  xhr.open("GET", "/readings", true);
  xhr.send();
}

if (!!window.EventSource) {
  var source = new EventSource('/events');
  
  source.addEventListener('open', function(e) {
    console.log("Events Connected");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
  }, false);
  
  source.addEventListener('message', function(e) {
    console.log("message", e.data);
  }, false);
  
  source.addEventListener('new_readings', function(e) {
    console.log("new_readings", e.data);
    var myObj = JSON.parse(e.data);
    console.log(myObj);
    gaugeTemp.value = myObj.temperature;
    gaugeAmp.value = myObj.amperage;
    gaugeHorizAmp.value = myObj.amperage;
  }, false);
}