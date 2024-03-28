const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".ck-content": {
      "ol, ul": {
        "@apply pl-5": {}
      }
    },
    "html:root": {
      "--ck-spacing-unit": "0.5rem",
      "--ck-border-radius": theme("borderRadius.md"),
      "--ck-color-base-text": theme("colors.slate.800"),
      "--ck-color-base-border": theme("colors.slate.300"),
      "--ck-color-button-default-hover-background": theme("colors.slate.100"),
      "--ck-color-button-on-color": theme("colors.custom.500"),
      "--ck-color-widget-type-around-button-active": theme("colors.custom.500"),
      "--ck-color-base-active": theme("colors.custom.500"),
      "--ck-color-button-on-background": theme("colors.custom.100"),
      "--ck-color-button-on-hover-background": theme("colors.custom.50"),
      "--ck-color-base-action": theme("colors.green.500"),
      "--ck-focus-outer-shadow": "none",
      "--ck-inner-shadow": "none"
    }
  });
});
