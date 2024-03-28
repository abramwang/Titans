const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents }) {
  addComponents({
    ".gslide-inline": {
      "@apply !bg-transparent overflow-hidden max-h-full !shadow-none items-center":
        {}
    },
    ".clip-triangle": {
      clipPath: "polygon(50% 0%, 0% 100%, 100% 100%)"
    }
  });
});
