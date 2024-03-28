const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".btn": {
      "@apply inline-block py-2 px-4 text-center border rounded-md border-transparent bg-transparent text-sm transition-all duration-200 ease-linear":
        {}
    }
  });
});
