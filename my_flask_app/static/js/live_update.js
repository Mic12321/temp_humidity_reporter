const socket = io();

let countdown = 5; // seconds
const countdownEl = document.getElementById("countdown");
// Decrease countdown every second
function tick() {
  countdown--;
  if (countdown <= 0) {
    countdown = 0;
  }
  countdownEl.textContent = countdown;
}

setInterval(tick, 1000);

socket.on("mqtt_message", function (msg) {
  document.getElementById("temp").textContent = msg.temperature.toFixed(1);
  document.getElementById("hum").textContent = msg.humidity.toFixed(1);
  const now = new Date();
  document.getElementById("time").textContent = now.toLocaleString();
  countdown = 5; // reset countdown
  countdownEl.textContent = countdown;
});
