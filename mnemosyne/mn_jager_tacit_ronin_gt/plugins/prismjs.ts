const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents }) {
  addComponents({
    ':not(pre) > code[class*="language-"], pre[class*="language-"]': {
      "@apply dark:bg-zink-500": {}
    }
  });
});
