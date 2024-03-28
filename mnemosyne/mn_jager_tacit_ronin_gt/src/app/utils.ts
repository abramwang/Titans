export const setAttribute = (key: string, value: string) => {
  document.documentElement.setAttribute(key, value);
};

export const colors: { value: string; title: string }[] = [
  { value: "custom", title: "Custom" },
  { value: "yellow", title: "Yellow" },
  { value: "green", title: "Green" },
  { value: "red", title: "Red" },
  { value: "orange", title: "Orange" },
  { value: "purple", title: "Purple" },
  { value: "sky", title: "Sky" },
  { value: "slate", title: "Slate" },
];

// rgb to hex convert
function rgbToHex(rgb: any) {
  // Extract RGB values using regular expressions
  const rgbValues = rgb.match(/\d+/g);

  if (rgbValues.length === 3) {
    var [r, g, b] = rgbValues.map(Number);
  }
  // Ensure the values are within the valid range (0-255)
  r = Math.max(0, Math.min(255, r));
  g = Math.max(0, Math.min(255, g));
  b = Math.max(0, Math.min(255, b));

  // Convert each component to its hexadecimal representation
  const rHex = r.toString(16).padStart(2, "0");
  const gHex = g.toString(16).padStart(2, "0");
  const bHex = b.toString(16).padStart(2, "0");

  // Combine the hexadecimal values with the "#" prefix
  const hexColor = `#${rHex}${gHex}${bHex}`;

  return hexColor.toUpperCase(); // Convert to uppercase for consistency
}

export const getChartColorsArray = (colors: any) => {
  if (colors) {
    const parsedColors = JSON.parse(colors);
    const mappedColors = parsedColors.map((value: any) => {
      const newValue = value.replace(/\s/g, "");
      if (!newValue.includes("#")) {
        const element = document.querySelector(newValue);
        if (element) {
          const styles = window.getComputedStyle(element);
          const backgroundColor = styles.backgroundColor;
          return backgroundColor || newValue;
        } else {
          const divElement = document.createElement("div");
          divElement.className = newValue;
          document.body.appendChild(divElement);

          const styles = window.getComputedStyle(divElement);
          const backgroundColor = styles.backgroundColor.includes("#")
            ? styles.backgroundColor
            : rgbToHex(styles.backgroundColor);
          return backgroundColor || newValue;
        }
      } else {
        return newValue;
      }
    });
    return mappedColors;
  } else {
    console.warn(`chart-colors attribute not found`);
  }
};
