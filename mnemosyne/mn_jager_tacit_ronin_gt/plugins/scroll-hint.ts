const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".scroll-hint-icon": {
      "@apply bg-slate-400 dark:bg-zink-400": {},
      height: "90px",

      ".scroll-hint-text": {
        "@apply text-xs text-white": {}
      }
    },

    ":root": {
      "--plyr-color-main": theme("colors.custom.500")
    }
  });
});
