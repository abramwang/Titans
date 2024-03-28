const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents }) {
  addComponents({
    ".popper": {
      "@apply !-top-3": {}
    }
  });
});
