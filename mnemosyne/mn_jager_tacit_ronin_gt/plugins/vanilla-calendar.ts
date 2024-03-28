const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents }) {
  addComponents({
    ".vanilla-calendar": {
      "@apply bg-white dark:!bg-zink-700 !w-full": {},

      button: {
        "&:focus-visible": {
          "@apply outline-orange-400": {}
        }
      }
    },

    ".vanilla-calendar-arrow": {
      "@apply transition-all duration-200 ease-linear before:bg-slate-500 dark:before:bg-zink-200 hover:before:bg-slate-800 dark:hover:before:bg-zink-50":
        {}
    },

    ".vanilla-calendar-header__content, .vanilla-calendar-month, .vanilla-calendar-year":
      {
        "@apply text-slate-800 dark:!text-zink-50": {}
      },

    ".vanilla-calendar-month, .vanilla-calendar-year": {
      "@apply transition-all duration-200 ease-linear hover:text-slate-500 dark:hover:text-zink-200":
        {},

      "&.vanilla-calendar-month_not-active, &.vanilla-calendar-year_not-active":
        {
          "@apply text-slate-500 dark:text-zink-200": {}
        },

      "&.vanilla-calendar-month_disabled, &.vanilla-calendar-year_disabled": {
        "@apply text-slate-400 dark:text-zink-300": {}
      }
    },

    ".vanilla-calendar-months__month": {
      "@apply bg-white dark:bg-zink-700 text-slate-600 dark:text-zink-50 hover:bg-slate-100 dark:hover:bg-zink-600":
        {},

      "&.vanilla-calendar-months__month_selected, &.vanilla-calendar-months__month_selected:hover":
        {
          "@apply text-white bg-custom-500": {}
        },

      "&.vanilla-calendar-months__month_disabled, &.vanilla-calendar-months__month_disabled:hover":
        {
          "@apply text-slate-500 dark:text-zink-200": {}
        }
    },

    ".vanilla-calendar-years__year": {
      "@apply bg-white dark:bg-zink-700 text-slate-700 dark:text-zink-100 hover:bg-slate-100 dark:hover:bg-zink-600":
        {},

      "&.vanilla-calendar-years__year_selected, &.vanilla-calendar-years__year_selected:hover":
        {
          "@apply bg-custom-500 text-white": {}
        },

      "&.vanilla-calendar-years__year_disabled, &.vanilla-calendar-years__year_disabled:hover":
        {
          "@apply text-slate-500 dark:text-zink-200": {}
        }
    },

    ".vanilla-calendar-week-numbers__title, .vanilla-calendar-week-number": {
      "@apply text-slate-500 dark:text-zink-200": {}
    },

    ".vanilla-calendar-week-number": {
      "@apply hover:text-slate-800 dark:hover:text-zink-50": {}
    },

    ".vanilla-calendar-week__day": {
      "@apply text-slate-500 dark:text-zink-200": {}
    },

    ".vanilla-calendar-week__day_weekend, .vanilla-calendar-day__btn_weekend, .vanilla-calendar-day__btn_holiday, .vanilla-calendar-day__btn_weekend.vanilla-calendar-day__btn_today, .vanilla-calendar-day__btn_holiday.vanilla-calendar-day__btn_today":
      {
        "@apply text-red-500": {}
      },

    ".vanilla-calendar-day__btn": {
      "@apply bg-white dark:bg-zink-700 text-slate-800 dark:text-zink-50 hover:bg-slate-100 dark:hover:bg-zink-600":
        {}
    },

    ".vanilla-calendar-day__btn_today": {
      "@apply bg-slate-100 dark:bg-zink-600 text-custom-500 dark:text-custom-500 hover:text-custom-500":
        {},

      "&.vanilla-calendar-day__btn_prev, &.vanilla-calendar-day__btn_next": {
        "@apply text-slate-500 dark:text-zink-200": {}
      }
    },

    ".vanilla-calendar-day__btn_prev, .vanilla-calendar-day__btn_next, .vanilla-calendar-day__btn_disabled":
      {
        "@apply text-slate-500 dark:text-zink-200": {}
      },

    ".vanilla-calendar-day__btn_hover, .vanilla-calendar-day__btn_weekend:hover, .vanilla-calendar-day__btn_holiday:hover, .vanilla-calendar-day__btn_weekend.vanilla-calendar-day__btn_hover, .vanilla-calendar-day__btn_holiday.vanilla-calendar-day__btn_hover":
      {
        "@apply bg-slate-100 dark:bg-zink-600": {}
      },

    ".vanilla-calendar-day__btn_weekend.vanilla-calendar-day__btn_disabled, .vanilla-calendar-day__btn_holiday.vanilla-calendar-day__btn_disabled, .vanilla-calendar-day__btn_weekend.vanilla-calendar-day__btn_today.vanilla-calendar-day__btn_disabled, .vanilla-calendar-day__btn_holiday.vanilla-calendar-day__btn_today.vanilla-calendar-day__btn_disabled":
      {
        "@apply text-slate-500 dark:text-zink-200": {}
      },

    ".vanilla-calendar-day__btn_weekend, .vanilla-calendar-day__btn_holiday": {
      "&.vanilla-calendar-day__btn_prev, &.vanilla-calendar-day__btn_next": {
        "@apply bg-white text-slate-500 dark:bg-zink-700 dark:text-zink-200": {}
      },

      "&.vanilla-calendar-day__btn_prev, &.vanilla-calendar-day__btn_next": {
        "@apply hover:bg-slate-200 dark:hover:bg-zink-600": {},

        "&.vanilla-calendar-day__btn_hover": {
          "@apply bg-slate-200 dark:bg-zink-600": {}
        },

        "&.vanilla-calendar-day__btn_today, &.vanilla-calendar-day__btn_disabled":
          {
            "@apply text-slate-500 dark:text-zink-200": {}
          },

        "&.vanilla-calendar-day__btn_selected": {
          "@apply text-white bg-red-500 hover:bg-red-500 hover:text-white": {},

          "&.vanilla-calendar-day__btn_intermediate": {
            "@apply text-white bg-red-500 hover:bg-red-500 hover:text-white": {}
          }
        },

        "&.vanilla-calendar-day__btn_selected": {
          "@apply text-custom-500 bg-custom-200 dark:bg-custom-500/20 hover:bg-custom-200 dark:hover:bg-custom-500/20 hover:text-custom-500":
            {},

          "&.vanilla-calendar-day__btn_intermediate": {
            "@apply text-custom-500 bg-custom-200 dark:bg-custom-500/20 hover:bg-custom-200 dark:hover:bg-custom-500/20 hover:text-custom-500":
              {}
          }
        }
      }
    },

    ".vanilla-calendar-day__btn_selected": {
      "@apply text-white bg-custom-500 hover:bg-custom-500 hover:text-white":
        {},

      "&.vanilla-calendar-day__btn_intermediate": {
        "@apply text-white bg-custom-500 hover:bg-custom-500 hover:text-white":
          {}
      },

      "&.vanilla-calendar-day__btn_prev, &.vanilla-calendar-day__btn_next": {
        "@apply text-custom-500 bg-custom-200 dark:bg-custom-500/10 hover:bg-custom-200 dark:hover:bg-custom-500/10 hover:text-custom-500":
          {},

        "&.vanilla-calendar-day__btn_intermediate": {
          "@apply text-slate-500 bg-slate-100 dark:bg-zink-600 dark:text-zink-200 hover:bg-slate-100 dark:hover:bg-zink-600 hover:text-slate-500 dark:hover:text-zink-200":
            {}
        }
      }
    },

    ".vanilla-calendar-day__popup": {
      "@apply text-slate-800 dark:text-zink-50 before:bg-white shadow dark:before:bg-zink-700 after:border-b-white dark:after:border-b-zink-700":
        {}
    },

    ".vanilla-calendar-time": {
      "@apply border-slate-200 dark:border-zink-500": {}
    },

    ".vanilla-calendar-time__hours": {
      "@apply after:text-slate-800 dark:after:text-zink-50": {}
    },

    ".vanilla-calendar-time__hours, .vanilla-calendar-time__minutes": {
      input: {
        "@apply text-slate-800 dark:text-zink-50 hover:bg-slate-100 dark:hover:bg-zink-600 focus-visible:outline-yellow-500":
          {},

        "&.vanilla-calendar-is-focus": {
          "@apply bg-slate-100 dark:bg-zink-600": {}
        }
      }
    },

    ".vanilla-calendar-time__keeping": {
      "@apply text-slate-500 dark:text-zink-200 hover:bg-slate-100 dark:hover:bg-zink-600 focus-visible:outline-yellow-500":
        {}
    },

    ".vanilla-calendar-time__range": {
      "@apply before:bg-custom-500/20 after:bg-custom-500/20 hover:border-slate-200 dark:hover:border-zink-500":
        {},

      input: {
        "@apply bg-white dark:bg-zink-700": {}
      }
    }
  });
});
