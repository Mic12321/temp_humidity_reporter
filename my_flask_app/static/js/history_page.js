import { formatLocalDate } from './utils/time_utils.js';

document.addEventListener('DOMContentLoaded', () => {
  document.querySelectorAll('td.timestamp').forEach(td => {
    const utcStr = td.dataset.initial;
    if (utcStr) {
      td.textContent = formatLocalDate(utcStr);
    }
  });
});