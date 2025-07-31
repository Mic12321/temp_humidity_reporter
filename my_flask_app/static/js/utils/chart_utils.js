export function setActiveButton(range) {
  const buttons = ["year", "week", "day", "hour"];
  buttons.forEach((r) => {
    const btn = document.getElementById(`btn-${r}`);
    if (btn) btn.classList.remove("active-range");
  });
  const activeBtn = document.getElementById(`btn-${range}`);
  if (activeBtn) activeBtn.classList.add("active-range");
}

export function getTimeUnit(range) {
  switch (range) {
    case 'year':
      return 'month';
    case 'week':
      return 'day';
    case 'day':
      return 'hour';
    case 'hour':
      return 'minute';
    default:
      return 'hour';
  }
}
