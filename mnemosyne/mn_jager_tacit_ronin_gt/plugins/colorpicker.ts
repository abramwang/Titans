const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".pickr": {
      ".pcr-button": {
        "@apply h-20 w-full rounded-md": {},

        "&::after, &::before": {
          "@apply rounded-md": {}
        }
      }
    },
    ".pcr-app": {
      "@apply rounded-md shadow-md border border-slate-200": {},
      zIndex: 999,
      "&[data-theme=classic]": {
        ".pcr-selection": {
          ".pcr-color-preview": {
            "@apply mr-3 ml-0": {}
          },

          ".pcr-color-chooser, .pcr-color-opacity": {
            "@apply ml-3 mr-0": {}
          }
        }
      },
      '&[data-theme="monolith"]': {
        ".pcr-result": {
          minWidth: theme("width.full")
        },
        ".pcr-interaction": {
          ".pcr-type.active": {
            "@apply bg-custom-500": {}
          },
          ".pcr-result": {
            "@apply bg-white text-slate-800 border border-slate-200 rounded-md":
              {}
          },

          input: {
            "@apply rounded-md": {},

            "&:focus": {
              "@apply shadow-none bg-white border-slate-200": {}
            }
          },

          ".pcr-save": {
            "@apply bg-green-500": {}
          },

          ".pcr-clear, .pcr-cancel": {
            "@apply bg-red-500": {}
          }
        }
      }
    }
  });
});
