const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".multiselect-wrapper": {
      "@apply border-0": {},

      ".multiselect, .multiselect-dropdown": {
        "@apply border border-slate-200 bg-white dark:border-zink-500 dark:bg-zink-700":
          {},
      },

      ".multiselect-option": {
        "@apply text-slate-700 dark:text-zink-100 dark:hover:text-zink-50 transition-all duration-200 ease-linear hover:bg-custom-500 hover:text-white":
          {},
      },

      ".multiselect-option.is-pointed": {
        "@apply bg-custom-500 hover:text-white": {},
      },

      ".multiselect-tags-search": {
        flex: "0 0 auto",
        "@apply py-2 px-4 text-slate-700 dark:text-zink-100 bg-white dark:bg-zink-700 font-public text-base rounded-md mb-4 placeholder-slate-400 dark:placeholder-zink-300":
          {},
      },
    },
  });
});
