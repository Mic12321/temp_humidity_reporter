import { setActiveButton, getTimeUnit } from './utils/chart_utils.js';

let chart;

document.addEventListener('DOMContentLoaded', () => {
  const ranges = ['year', 'week', 'day', 'hour'];
  ranges.forEach(range => {
    document.getElementById(`btn-${range}`).addEventListener('click', () => {
      fetchAndPlot(range);
    });
  });

  fetchAndPlot('day');
});

function fetchAndPlot(range) {
  setActiveButton(range);

  fetch(`/api/history?range=${range}`)
    .then(res => res.json())
    .then(data => {
      if (!Array.isArray(data)) {
        alert(data.error || "Invalid data received");
        return;
      }

      const labels = data.map(d =>
          luxon.DateTime.fromISO(d.timestamp.endsWith('Z') ? d.timestamp : d.timestamp + 'Z')
              .setZone('local')
              .toJSDate()
      );
      const temps = data.map(d => d.temperature);
      const hums = data.map(d => d.humidity);

      if (chart) chart.destroy();

      document.getElementById("chartTitle").textContent = {
        year: "Sensor History – Last 12 Months",
        week: "Sensor History – Last 7 Days",
        day: "Sensor History – Last 24 Hours",
        hour: "Sensor History – Last Hour",
      }[range] || "Sensor History";

      chart = new Chart(document.getElementById("historyChart"), {
        type: "line",
        data: {
          labels,
          datasets: [
            {
              label: "Temperature (°C)",
              data: temps,
              borderColor: "red",
              backgroundColor: "rgba(255, 99, 132, 0.2)",
              yAxisID: "y",
            },
            {
              label: "Humidity (%)",
              data: hums,
              borderColor: "blue",
              backgroundColor: "rgba(54, 162, 235, 0.2)",
              yAxisID: "y1",
            },
          ],
        },
        options: {
          responsive: true,
          scales: {
            x: {
              type: "time",
              time: {
                tooltipFormat: "MMM d HH:mm",
                unit: getTimeUnit(range),
              },
                adapters: {
                    date: {
                    zone: 'local', 
                },
            },
              title: {
                display: true,
                text: "Timestamp",
              },

            },
            y: {
              type: "linear",
              position: "left",
              title: {
                display: true,
                text: "Temperature (°C)",
              },
            },
            y1: {
              type: "linear",
              position: "right",
              grid: {
                drawOnChartArea: false,
              },
              title: {
                display: true,
                text: "Humidity (%)",
              },
            },
          },
        },
      });
    })
    .catch(err => {
      console.error(err);
      alert("Failed to load data.");
    });
}

