const socket = io();

const timeEl = document.getElementById("time");
const countdownEl = document.getElementById("countdown");
const errorBox = document.getElementById("error-message");

let lastValidTime = timeEl.dataset.initial || null;
let errorSinceTime = null;
let countdown = 60;

if (lastValidTime) {
  timeEl.textContent = `Last updated: ${lastValidTime}`;
} else {
  timeEl.textContent = "Waiting for data...";
}

function tick() {
  countdown = Math.max(0, countdown - 1);
  countdownEl.textContent = countdown;
}

setInterval(tick, 1000);

socket.on("mqtt_message", function (msg) {
  document.getElementById("temp").textContent = msg.temperature.toFixed(1);
  document.getElementById("hum").textContent = msg.humidity.toFixed(1);

  const now = new Date();
  lastValidTime = now.toLocaleString();
  timeEl.textContent = `Last updated: ${lastValidTime}`;

  countdown = 60;
  countdownEl.textContent = countdown;

  errorBox.style.display = "none";
  errorSinceTime = null;
});

socket.on("mqtt_error", function (msg) {
  const now = new Date();
  if (!errorSinceTime) {
    errorSinceTime = now.toLocaleString();
  }

  if (lastValidTime) {
    timeEl.textContent = `Last updated: ${lastValidTime} — Sensor error since: ${errorSinceTime}`;
  } else {
    timeEl.textContent = `Sensor error since: ${errorSinceTime}`;
  }

  errorBox.textContent = msg.error;
  errorBox.style.display = "block";

  countdown = 60;
  countdownEl.textContent = countdown;
});
