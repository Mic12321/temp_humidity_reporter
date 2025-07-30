const socket = io();

const timeEl = document.getElementById("time");
const countdownEl = document.getElementById("countdown");
const errorBox = document.getElementById("error-message");
const tempEl = document.getElementById("temp");
const humEl = document.getElementById("hum");

let lastValidTime = timeEl.dataset.initial || null;
let errorSinceTime = null;
let countdown = 60;

timeEl.textContent = lastValidTime
  ? `Last updated: ${lastValidTime}`
  : "Waiting for data...";

function tick() {
  countdown = Math.max(0, countdown - 1);
  countdownEl.textContent = countdown;
}

setInterval(tick, 1000);

socket.on("mqtt_message", function (msg) {
  tempEl.textContent = msg.temperature.toFixed(1);
  humEl.textContent = msg.humidity.toFixed(1);

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

  timeEl.textContent = lastValidTime
    ? `Last updated: ${lastValidTime} — Sensor error since: ${errorSinceTime}`
    : `Sensor error since: ${errorSinceTime}`;

  errorBox.textContent = msg.error;
  errorBox.style.display = "block";

  countdown = 60;
  countdownEl.textContent = countdown;
});

document.addEventListener("DOMContentLoaded", () => {
  const timeEl = document.getElementById("time");
  if (timeEl && timeEl.dataset.initial) {
    // Parse the UTC time string into a Date object
    const utcDate = new Date(timeEl.dataset.initial);
    if (!isNaN(utcDate)) {
      // Format local date/time string as "dd/mm/yyyy, HH:MM:SS"
      const pad = (n) => n.toString().padStart(2, '0');
      const day = pad(utcDate.getDate());
      const month = pad(utcDate.getMonth() + 1);
      const year = utcDate.getFullYear();
      const hours = pad(utcDate.getHours());
      const minutes = pad(utcDate.getMinutes());
      const seconds = pad(utcDate.getSeconds());
      
      const localFormatted = `${day}/${month}/${year}, ${hours}:${minutes}:${seconds}`;
      timeEl.textContent = `Last updated: ${localFormatted}`;
    }
  }
});