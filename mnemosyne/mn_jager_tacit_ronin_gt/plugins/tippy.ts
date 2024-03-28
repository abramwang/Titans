const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".tippy-box[data-animation=fade][data-state=hidden]": {
      "@apply opacity-0": {}
    },
    "[data-tippy-root]": {
      maxWidth: "calc(100vw - 10px)"
    },
    ".tippy-box": {
      "@apply relative text-white rounded-md text-xs bg-slate-800 outline-0 whitespace-normal leading-snug dark:bg-zink-500":
        {},
      transitionProperty: "transform, visibility, opacity",

      "&[data-placement^=top]": {
        ">.tippy-arrow": {
          "@apply bottom-0": {},

          "&::before": {
            bottom: "-7px",
            "@apply left-0": {},
            left: 0,
            borderWidth: "8px 8px 0",
            borderTopColor: "initial",
            transformOrigin: "center top"
          }
        }
      },

      "&[data-placement^=bottom]": {
        ">.tippy-arrow": {
          "@apply top-0": {},

          "&::before": {
            top: "-7px",
            "@apply left-0": {},
            borderWidth: "0 8px 8px",
            borderTopColor: "initial",
            transformOrigin: "center bottom"
          }
        }
      },

      "&[data-placement^=left]": {
        ">.tippy-arrow": {
          "@apply right-0": {},

          "&::before": {
            right: "-7px",
            borderWidth: "8px 0 8px 8px",
            borderLeftColor: "initial",
            transformOrigin: "center left"
          }
        }
      },

      "&[data-placement^=right]": {
        ">.tippy-arrow": {
          "@apply left-0": {},

          "&::before": {
            left: "-7px",
            borderWidth: "8px 8px 8px 0",
            borderRightColor: "initial",
            transformOrigin: "center right"
          }
        }
      },

      "&[data-inertia][data-state=visible]": {
        transitionTimingFunction: "cubic-bezier(.54, 1.5, .38, 1.11)"
      }
    },

    ".tippy-arrow": {
      "@apply w-4 h-4 text-slate-800 dark:text-zink-500 before:absolute before:border-solid before:border-transparent":
        {}
    },
    ".tippy-content": {
      "@apply relative py-1 px-2": {},
      zIndex: 1
    }
  });
});
