const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents }) {
  addComponents({
    ".glightbox-clean ": {
      ".gslide-description": {
        "@apply bg-white dark:bg-zink-600": {}
      },

      ".gslide-title, .vel-img-title": {
        "@apply text-slate-800 dark:text-zink-50 mb-4": {}
      }
    }
  });
});
