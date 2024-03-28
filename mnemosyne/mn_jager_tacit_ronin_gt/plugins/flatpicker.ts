const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".flatpickr-calendar": {
      "@apply text-sm bg-white dark:bg-zink-600 rounded-md dark:shadow-md dark:shadow-zink-500":
        {},
      width: "307.875px",

      "&.open": {
        zIndex: 9999
      },

      ".flatpickr-rContainer": {
        ".flatpickr-days": {
          "@apply border-t border-slate-200 dark:border-zink-500": {}
        }
      },
      ".flatpickr-time": {
        "@apply border border-slate-200 dark:border-zink-500 rounded-md": {},

        input: {
          "@apply text-slate-700 dark:text-zink-50": {}
        }
      },

      "&.hasTime .flatpickr-time": {
        "@apply border-t border-slate-200 dark:border-zink-500": {}
      },

      "&.arrowTop::before,  &.arrowTop::after": {
        "@apply border-b border-b-slate-200 dark:border-b-zink-500": {}
      },

      "&.arrowBottom::before,  &.arrowBottom::after": {
        "@apply border-t border-slate-200 dark:border-zink-500": {}
      }
    },
    ".flatpickr-months": {
      "@apply bg-transparent flex items-center h-12": {},

      ".flatpickr-month": {
        "@apply text-slate-500 dark:text-zink-200 dark:fill-zink-500 fill-slate-600 h-12":
          {},

        ".flatpickr-current-month": {
          "@apply text-15 h-auto pt-0 flex items-center top-0 bottom-0": {},

          ".flatpickr-monthDropdown-months": {
            "&:hover": {
              "@apply bg-transparent text-slate-900 dark:text-zink-50": {}
            }
          }
        }
      },

      ".flatpickr-prev-month, .flatpickr-next-month": {
        "@apply text-slate-500 fill-slate-500 h-8 w-8 flex items-center hover:text-slate-600 dark:hover:text-zink-100 top-2 border border-slate-200 dark:border-zink-500 dark:text-zink-200 rounded-md":
          {},

        "&.flatpickr-prev-month": {
          "@apply left-2": {}
        },

        "&.flatpickr-next-month": {
          "@apply right-2": {}
        },

        svg: {
          "@apply h-10 w-10": {}
        },

        "&:hover": {
          svg: {
            "@apply fill-custom-500": {}
          }
        }
      }
    },

    ".flatpickr-weekdays": {
      "@apply h-8": {}
    },

    "span.flatpickr-weekday": {
      "@apply font-semibold text-slate-500 dark:text-zink-200": {}
    },

    ".numInputWrapper": {
      "&:hover": {
        "@apply bg-transparent": {}
      }
    },

    ".flatpickr-day": {
      "@apply text-slate-800 dark:text-zink-50 rounded-md h-8 leading-7 m-1":
        {},
      maxWidth: theme("width.8"),

      "&:hover, &:focus": {
        "@apply text-slate-500 bg-slate-100 dark:text-zink-200 dark:bg-zink-500":
          {}
      },
      "&.inRange, &.prevMonthDay.inRange, &.nextMonthDay.inRange, &.today.inRange, &.prevMonthDay.today.inRange, &.nextMonthDay.today.inRange, &:hover, &.prevMonthDay:hover, &.nextMonthDay:hover, &:focus, &.prevMonthDay:focus, &.nextMonthDay:focus":
        {
          "@apply text-slate-500 bg-slate-100 border-slate-200 dark:text-zink-200 dark:border-zink-500 dark:bg-zink-500":
            {}
        },

      "&.today": {
        "@apply text-custom-50 bg-custom-500 border-custom-500": {},
        "&:hover, &:focus": {
          "@apply text-custom-50 bg-custom-600 border-custom-600": {}
        }
      },

      "&.selected, &.startRange, &.endRange, &.selected.inRange, &.startRange.inRange, &.endRange.inRange, &.selected:focus, &.startRange:focus, &.endRange:focus, &.selected:hover, &.startRange:hover, &.endRange:hover, &.selected.prevMonthDay, &.startRange.prevMonthDay, &.endRange.prevMonthDay, &.selected.nextMonthDay, &.startRange.nextMonthDay, &.endRange.nextMonthDay":
        {
          "@apply text-custom-50 bg-custom-500 border-custom-500": {}
        },

      "&.inRange, &.week.selected": {
        boxShadow: `-10px 0 0 ${theme("colors.slate.100")}, 10px 0 0 ${theme(
          "colors.slate.100"
        )}`,
        borderColor: "transparent"
      },

      "&:is(.flatpickr-disabled, .flatpickr-disabled:hover, .prevMonthDay, .nextMonthDay, .notAllowed, .notAllowed.prevMonthDay, .notAllowed.nextMonthDay)":
        {
          "@apply text-slate-300 dark:text-zink-300": {}
        },

      "&.flatpickr-disabled, &.flatpickr-disabled:hover": {
        "@apply text-slate-300 dark:text-zink-300": {}
      }
    },

    '[data-mode="dark"]': {
      ".flatpickr-day": {
        "&.inRange, &.week.selected": {
          boxShadow: `-10px 0 0 ${theme("colors.zink.500")}, 10px 0 0 ${theme(
            "colors.zink.500"
          )}`
        }
      },

      ".flatpickr-weekwrapper": {
        ".flatpickr-weeks": {
          boxShadow: `1px 0 0 ${theme("colors.zink.500")}`,
          borderTop: `1px solid ${theme("colors.zink.500")}`
        }
      }
    },

    ".flatpickr-weekwrapper": {
      ".flatpickr-weekday": {
        "@apply h-8": {}
      },

      ".flatpickr-weeks": {
        boxShadow: `1px 0 0 ${theme("colors.slate.200")}`,
        borderTop: `1px solid ${theme("colors.slate.200")}`,

        ".flatpickr-day": {
          "@apply h-8 leading-7 m-0 h-10": {},
          lineHeight: "40px"
        }
      },

      span: {
        "&.flatpickr-day, &.flatpickr-day:hover": {
          "@apply text-slate-300 dark:text-zink-300": {}
        }
      }
    },

    ".flatpickr-current-month input.cur-year[disabled], .flatpickr-current-month input.cur-year[disabled]:hover":
      {
        "@apply text-slate-700 dark:text-zink-100": {}
      },
    ".flatpickr-time input:hover, .flatpickr-time .flatpickr-am-pm:hover, .flatpickr-time input:focus, .flatpickr-time .flatpickr-am-pm:focus":
      {
        "@apply bg-slate-100 dark:bg-zink-500": {}
      },

    ".flatpickr-time .flatpickr-time-separator, .flatpickr-time .flatpickr-am-pm":
      {
        "@apply text-slate-500 dark:text-zink-200": {}
      }
  });
});
