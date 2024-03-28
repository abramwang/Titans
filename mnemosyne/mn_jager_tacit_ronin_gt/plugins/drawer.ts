const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents }) {
  addComponents({
    "[drawer-end='true']": {
      "&.show": {
        "@apply ltr:translate-x-full rtl:-translate-x-full": {}
      }
    },
    "[drawer-start='true']": {
      "&.show": {
        "@apply rtl:translate-x-full ltr:-translate-x-full": {}
      }
    },
    "[drawer-bottom='true']": {
      "&.show": {
        "@apply translate-y-full": {}
      }
    },
    "[drawer-top='true']": {
      "&.show": {
        "@apply -translate-y-full": {}
      }
    },
    "[modal-center='true']": {
      "@apply opacity-100 top-1/2": {},

      "&.show": {
        "@apply opacity-0": {},
        top: "45%"
      }
    },
    "[modal-top='true']": {
      "@apply opacity-100": {},
      top: "4%",
      "&.show": {
        "@apply opacity-0 top-0": {}
      }
    },
    "[modal-bottom='true']": {
      bottom: "4%",
      "@apply opacity-100": {},
      "&.show": {
        "@apply opacity-0 bottom-0": {}
      }
    }
  });
});
