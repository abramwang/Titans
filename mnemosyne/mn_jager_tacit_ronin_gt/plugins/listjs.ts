const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".listjs-pagination": {
      "@apply mb-0 justify-end gap-2": {},

      li: {
        ".page": {
          "@apply flex items-center justify-center h-8 w-8 border border-slate-200 dark:border-zink-500 rounded-md":
            {}
        },

        "&.active": {
          ".page": {
            "@apply text-white bg-custom-500 border-custom-500": {}
          }
        }
      }
    },

    ".pagination-wrap": {
      "@apply flex items-center select-none": {}
    }
  });
});
