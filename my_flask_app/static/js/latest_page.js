import { formatLocalDate } from './utils/time_utils.js';

document.addEventListener("DOMContentLoaded", () => {
  const socket = io();

  const timeEl = document.getElementById("time");
  const countdownEl = document.getElementById("countdown");
  const errorBox = document.getElementById("error-message");
  const tempEl = document.getElementById("temp");
  const humEl = document.getElementById("hum");

  let lastValidTime = timeEl.dataset.initial || null;
  let errorSinceTime = null;
  let countdown = 60;

  // Set initial timestamp text
  if (lastValidTime) {
    timeEl.textContent = `Last updated: ${formatLocalDate(lastValidTime)}`;
  } else {
    timeEl.textContent = "Waiting for data...";
  }

  // Countdown updater
  function tick() {
    countdown = Math.max(0, countdown - 1);
    countdownEl.textContent = countdown;
  }
  setInterval(tick, 1000);

  // On new sensor data
  socket.on("mqtt_message", function (msg) {
    tempEl.textContent = msg.temperature.toFixed(1);
    humEl.textContent = msg.humidity.toFixed(1);

    const now = new Date();
    lastValidTime = now.toISOString();
    timeEl.textContent = `Last updated: ${formatLocalDate(lastValidTime)}`;

    countdown = 60;
    countdownEl.textContent = countdown;

    errorBox.style.display = "none";
    errorBox.textContent = "";
    errorSinceTime = null;
  });

  // On error
  socket.on("mqtt_error", function (msg) {
    const now = new Date();
    if (!errorSinceTime) {
      errorSinceTime = now.toISOString();
    }

    const lastText = lastValidTime
      ? `Last updated: ${formatLocalDate(lastValidTime)} — `
      : "";

    timeEl.textContent = `${lastText}Sensor error since: ${formatLocalDate(errorSinceTime)}`;

    errorBox.textContent = msg.error;
    errorBox.style.display = "block";

    countdown = 60;
    countdownEl.textContent = countdown;
  });
});
