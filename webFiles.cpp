/**
 * @file webFiles.cpp
 * @brief Server
 */

#include "webFiles.h"

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <title>Smart Greenhouse</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" type="text/css" href="style.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

</head>
<meta charset="UTF-8">

<body>

    <div class="topnav">
        <div class="plant-greeting">
            <p style="margin: 0; font-size: 1.2rem; color: white;">
                <strong>Smart Greenhouse</strong>
            </p>
        </div>
    </div>

    <div class="topnav2">
        <h1>Change Setpoint ⚙️</h1>
    </div>
    <div class="content">
        <div class="card-grid">
            <div class="card">
                <p class="card-title">Humidity</p>
                <p class="switch">
                    <input type="number" id="setHum" min="0" max="100" step="5">
                    <button onclick="sendHum(this)">Set</button>
                </p>
            </div>
            <div class="card">
                <p class="card-title">Light</p>
                <p class="switch">
                    <input type="number" id="setLight" min="0" max="100" step="5">
                    <button onclick="sendLight(this)">Set</button>
                </p>
            </div>
            <div class="card">
                <p class="card-title">Temperature</p>
                <p class="switch">
                    <input type="number" id="setTemp" min="10" max="30" step="1">
                    <button onclick="sendTemp(this)">Set</button>
                </p>
            </div>
        </div>
    </div>

    <div class="topnav2">
        <h1>Sensor Readings 📊</h1>
    </div>
    <div class="content">
        <div class="card-grid">
            <div class="card">
                <p class="card-title"> Humidity 💧</p>
                <p class="reading"><span id="humidity"></span> &percnt;</p>
                <div id="hum-indicator" class="hum_visual-indicator"></div>
            </div>
            <div class="card">
                <p class="card-title"> Light ☀️</p>
                <p class="reading"><span id="light"></span> %</p>
                <div id="light-indicator" class="light_visual-indicator"></div>
            </div>
            <div class="card">
                <p class="card-title"><i class="fas fa-thermometer-threequarters" style="color:#059e8a;"></i>
                    Temperature 🌡️</p>
                <p class="reading"><span id="temperature"></span> &deg;C</p>
                <div id="temp-indicator" class="temp_visual-indicator"></div>
            </div>
        </div>
    </div>

    <div class="topnav2">
        <h1>ThingSpeak Graph 📈 📉</h1>
    </div>
    <div class="content">
        <div class="card-grid">
            <div class="card">
                <p class="card-title">Humidity</p>
                <canvas id="thingspeakChart2"></canvas>
            </div>
            <div class="card">
                <p class="card-title">Temperature</p>
                <canvas id="thingspeakChart1"></canvas>
            </div>
        </div>
    </div>

    <script src="script.js"></script>

</body>

</html>

)rawliteral";

const char STYLE_CSS[] PROGMEM = R"rawliteral(
html {
    font-family: "Inter", "Segoe UI", Arial, sans-serif;
    background: #eef5f0;
}

body {
    margin: 0;
}

/* ===== HEADER ===== */
.topnav {
    background: linear-gradient(135deg, #1b5e20, #33691e);
    padding: 16px 24px;
    display: flex;
    align-items: center;
    gap: 16px;
    box-shadow: 0 6px 16px rgba(0,0,0,0.25);
}

.topnav h1 {
    font-size: 2rem;
    letter-spacing: 1px;
}

/* ===== SECTION HEADERS ===== */
.topnav2 {
    max-width: 1100px;
    margin: 40px auto 10px;
    background: none;
    box-shadow: none;
    padding: 0 10px;
}

.topnav2 h1 {
    color: #1b5e20;
    text-align: left;
    font-size: 1.6rem;
    border-left: 6px solid #43a047;
    padding-left: 14px;
}

/* ===== CONTENT ===== */
.content {
    padding: 10px 20px 40px;
}

/* ===== GRID ===== */
.card-grid {
    max-width: 1100px;
    margin: auto;
    display: grid;
    gap: 24px;
    grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
}

/* ===== CARDS ===== */
.card {
    background: #ffffff;
    border-radius: 18px;
    padding: 26px;
    box-shadow: 0 12px 28px rgba(0,0,0,0.12);
    position: relative;
    overflow: hidden;
}

/* decorative greenhouse line */
.card::before {
    content: "";
    position: absolute;
    top: 0;
    left: 0;
    height: 6px;
    width: 100%;
    background: linear-gradient(90deg, #66bb6a, #aed581);
}

.card-title {
    font-size: 1.2rem;
    font-weight: 600;
    color: #2e7d32;
    margin-bottom: 14px;
}

/* ===== SENSOR VALUES ===== */
.reading {
    font-size: 2.4rem;
    font-weight: 700;
    color: #1b5e20;
}

/* ===== CONTROL PANEL ===== */
.switch {
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 12px;
}

.switch input {
    width: 90px;
    padding: 10px;
    border-radius: 10px;
    border: 1px solid #c8e6c9;
    font-size: 16px;
}

.switch button {
    background: #43a047;
    color: white;
    border: none;
    border-radius: 10px;
    padding: 10px 18px;
    font-weight: 600;
    cursor: pointer;
    transition: background 0.2s;
}

.switch button:hover {
    background: #2e7d32;
}

/* ===== TEMP BAR ===== */
.temp_visual-indicator {
    height: 10px;
    margin-top: 16px;
    border-radius: 10px;
    background: linear-gradient(
        to right,
        rgba(255, 99, 132, 1) var(--indicator-width, 0%),
        #e0e0e0 var(--indicator-width, 0%) 100%
    );
}
.light_visual-indicator {
    height: 10px;
    margin-top: 16px;
    border-radius: 10px;
    background: linear-gradient(
        to right,
        #ffb300 var(--indicator-width, 0%),
        #e0e0e0 var(--indicator-width, 0%) 100%
    );
}
.hum_visual-indicator {
    height: 10px;
    margin-top: 16px;
    border-radius: 10px;
    background: linear-gradient(
        to right,
        rgba(5, 158, 138, 1) var(--indicator-width, 0%),
        #e0e0e0 var(--indicator-width, 0%) 100%
    );
}

/* ===== GRAPHS ===== */
canvas {
    margin-top: 14px;
    max-height: 260px;
}

/* ===== RESPONSIVE ===== */
@media (max-width: 600px) {
    .reading {
        font-size: 2rem;
    }
}

/* ********** button animation
/* click press */
.switch button:active {
    transform: scale(0.95);
}
/* success animation */
.switch button.sent {
    animation: sentPulse 0.55s ease;
}

@keyframes sentPulse {
    0% {
        transform: scale(1);
        box-shadow: 0 0 0 0 rgba(67,160,71,0.6);
    }
    50% {
        transform: scale(1.1);
        box-shadow: 0 0 0 8px rgba(50, 124, 53, 0);
    }
    100% {
        transform: scale(1);
        box-shadow: none;
    }
}

)rawliteral";

const char SCRIPT_JS[] PROGMEM = R"rawliteral(
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
}
function getReadings(){
    websocket.send("getReadings");
}

// ********** Animation click
function animateButton(btn) {
    btn.classList.remove("sent");
    void btn.offsetWidth;
    btn.classList.add("sent");
}

function sendTemp(btn) {
    const value = document.getElementById("setTemp").value;
    websocket.send("T:" + value);
    animateButton(btn);
}

function sendHum(btn) {
    const value = document.getElementById("setHum").value;
    websocket.send("H:" + value);
    animateButton(btn);
}

function sendLight(btn) {
    const value = document.getElementById("setLight").value;
    websocket.send("L:" + value);
    animateButton(btn);
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}
function onOpen(event) {
    console.log('Connection opened');
    getReadings();
    getTime();
}
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}
function updateIndicator(elementId, value, min, max) {
    const element = document.getElementById(elementId);
    if (!element) return;
    
    const percentage = ((value - min) / (max - min)) * 100;
    const clampedPercentage = Math.max(0, Math.min(100, percentage));
    element.style.setProperty('--indicator-width', `${clampedPercentage}%`);
}

function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);
    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
        if (key === "temperature") {
            updateIndicator("temp-indicator", myObj[key], 10, 30); // 10-30°C range
        }
         if (key === "humidity") {
            updateIndicator("hum-indicator", myObj[key], 0, 100); // range
        }
         if (key === "light") {
            updateIndicator("light-indicator", myObj[key], 0, 100); // range
        }
    }
}


let chartField1, chartField2;

async function fetchThingSpeakData() {
    const url = "https://api.thingspeak.com/channels/3226727/feeds.json?api_key=IGWBQXHCQF1OJ3R1&results=20";

    try {
        const response = await fetch(url);
        const data = await response.json();
        const feeds = data.feeds;

        const labels = feeds.map(f => f.created_at);
        const field1Values = feeds.map(f => parseFloat(f.field1) || 0);
        const field2Values = feeds.map(f => parseFloat(f.field2) || 0);

        if (!chartField1) {
            chartField1 = renderSingleChart('thingspeakChart1', '-', labels, field1Values, 'rgba(255, 99, 132, 1)');
            chartField2 = renderSingleChart('thingspeakChart2', '-', labels, field2Values, 'rgba(5, 158, 138, 1)');
        } else {
            chartField1.data.labels = labels;
            chartField1.data.datasets[0].data = field1Values;
            chartField1.update();

            chartField2.data.labels = labels;
            chartField2.data.datasets[0].data = field2Values;
            chartField2.update();
        }
    } catch (error) {
        console.error("Error fetching ThingSpeak data:", error);
    }
}

function renderSingleChart(canvasId, label, labels, data, color) {
    const ctx = document.getElementById(canvasId).getContext('2d');
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                label: label,
                data: data,
                borderColor: color,
                backgroundColor: color.replace('1)', '0.2)'),
                fill: true,
                tension: 0.3
            }]
        },
        options: {
            responsive: true,
            scales: {
                x: { ticks: { display: false } },
                y: { title: { display: true, text: 'Value' } }
            }
        }
    });
}

// Initial load
window.addEventListener('load', () => {
    fetchThingSpeakData();
    setInterval(fetchThingSpeakData, 10000); //10 seconds request new load
});

)rawliteral";
