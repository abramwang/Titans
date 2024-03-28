const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".choices": {
      "@apply relative overflow-hidden mb-6 text-base": {},

      "&:focus": {
        "@apply outline-none": {}
      },

      "&:last-child": {
        "@apply mb-0": {}
      },

      ".choices__placeholder": {
        "@apply mb-0 text-slate-400 dark:text-zink-200": {}
      },

      "&.is-disabled": {
        ".choices__inner, .choices__input": {
          "@apply bg-slate-100 dark:bg-zink-500 cursor-not-allowed": {}
        },

        ".choices__item": {
          "@apply cursor-not-allowed": {}
        }
      },

      ".choices__input": {
        "@apply mb-0 bg-white dark:bg-zink-700": {}
      },

      ".choices__heading": {
        "@apply border-b border-slate-200 text-slate-500 dark:border-zink-500 dark:text-zink-200":
          {}
      },

      "[hidden]": {
        "@apply hidden": {}
      },
      

      "&[data-type*=select-one]": {
        "@apply cursor-pointer": {},

        ".choices__inner": {
          "@apply pb-1": {}
        },

        ".choices__input": {
          "@apply block w-full py-1 px-4 border-b border-slate-200 dark:border-zink-500 bg-white dark:bg-zink-700  m-0":
            {}
        },

        ".choices__button": {
          "@apply p-0 absolute top-1/2 h-5 w-5 opacity-25 ltr:right-6 rtl:left-6 rounded-md -mt-2.5":
            {},
          backgroundImage: `url("data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjEiIGhlaWdodD0iMjEiIHZpZXdCb3g9IjAgMCAyMSAyMSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48ZyBmaWxsPSIjMDAwIiBmaWxsLXJ1bGU9ImV2ZW5vZGQiPjxwYXRoIGQ9Ik0yLjU5Mi4wNDRsMTguMzY0IDE4LjM2NC0yLjU0OCAyLjU0OEwuMDQ0IDIuNTkyeiIvPjxwYXRoIGQ9Ik0wIDE4LjM2NEwxOC4zNjQgMGwyLjU0OCAyLjU0OEwyLjU0OCAyMC45MTJ6Ii8+PC9nPjwvc3ZnPg==")`,
          backgroundSize: "8px",

          "&:hover, &:focus": {
            "@apply opacity-100": {}
          },

          "&:focus": {
            boxShadow: `0 0 0 2px ${theme("colors.custom.500")}`
          }
        },

        ".choices__item": {
          '&[data-value=""]': {
            ".choices__button": {
              "@apply hidden": {}
            }
          }
        },

        "&::after": {
          "@apply h-0 w-0 top-1/2 absolute border-solid ltr:right-3 rtl:right-auto rtl:left-3 border-t-slate-200 dark:border-t-zink-500 border-transparent":
            {},
          content: `' '`,
          borderWidth: "5px",
          marginTop: "-2.5px",
          pointerEvents: "none"
        },

        "&.is-open": {
          "&::after": {
            "@apply border-b-slate-200 dark:border-b-zink-500 border-transparent":
              {}
          }
        }
      },

      "&[data-type*=select-multiple], &[data-type*=text]": {
        ".choices__inner": {
          cursor: "text"
        },

        ".choices__button": {
          "@apply relative inline-block mt-0 ltr:-mr-1 rtl:-ml-1 mb-0 ltr:ml-2 ltr:pl-4 rtl:mr-2 rtl:pl-0 rtl:pr-2 ltr:border-l rtl:border-r border-green-500 opacity-75 rounded-none":
            {},
          backgroundImage:
            'url("data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjEiIGhlaWdodD0iMjEiIHZpZXdCb3g9IjAgMCAyMSAyMSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48ZyBmaWxsPSIjRkZGIiBmaWxsLXJ1bGU9ImV2ZW5vZGQiPjxwYXRoIGQ9Ik0yLjU5Mi4wNDRsMTguMzY0IDE4LjM2NC0yLjU0OCAyLjU0OEwuMDQ0IDIuNTkyeiIvPjxwYXRoIGQ9Ik0wIDE4LjM2NEwxOC4zNjQgMGwyLjU0OCAyLjU0OEwyLjU0OCAyMC45MTJ6Ii8+PC9nPjwvc3ZnPg==")',
          backgroundSize: "8px",
          width: "8px",
          lineHeight: 1,

          "&::hover, &:focus": {
            "@apply opacity-100": {}
          }
        }
      },

      ".choices__inner": {
        "@apply inline-block w-full py-1 px-2 text-sm align-top bg-white dark:bg-zink-700 border border-slate-200 dark:border-zink-500 rounded-md overflow-hidden":
          {},
        minHeight: "39px"
      },

      ".choices__list--dropdown, .choices__list[aria-expanded]": {
        "@apply bg-white dark:bg-zink-700": {}
      }
    },

    ".is-focused": {
      ".choices__inner": {
        "@apply border-slate-200": {}
      }
    },

    ".choices__list": {
      "@apply ltr:pl-0 rtl:pr-0 list-none m-0": {}
    },

    ".choices__list--single": {
      "@apply inline-block p-1 ltr:pr-4 rtl:pl-4 w-full": {}
    },

    ".choices__list--multiple": {
      "@apply inline": {},

      ".choices__item": {
        "@apply inline-block align-middle rounded-md py-1 px-2 font-medium mb-0 bg-custom-500 border border-custom-500 text-white break-all box-border":
          {},
        fontSize: "11px",
        marginRight: "3.75px",

        "&[data-deletable]": {
          paddingRight: "5px"
        },

        "&.is-highlighted": {
          "@apply bg-custom-500 border border-custom-500": {}
        }
      }
    },

    ".is-disabled": {
      ".choices__list--multiple": {
        ".choices__item": {
          "@apply bg-slate-300 dark:bg-zink-600 border border-slate-200 dark:border-zink-500":
            {}
        }
      }
    },

    ".choices__list--dropdown": {
      ".choices__list": {
        "&[aria-expanded]": {
          "@apply invisible absolute w-full bg-white border rounded-b-md border-slate-200 dark:border-zink-500 top-full overflow-hidden break-all":
            {},
          zIndex: 1,
          marginTop: "-1px",
          willChange: "visibility"
        }
      },
      ".choices__list, .choices__list[aria-expanded] .choices__list": {
        "@apply relative overflow-auto will-change-scroll": {},
        maxHeight: "220px"
      },

      ".choices__item, .choices__list[aria-expanded] .choices__item": {
        "@apply relative !p-2 !text-sm": {}
      }
    },

    ".choices__list--dropdown, .choices__list[aria-expanded]": {
      "@apply bg-white dark:bg-zink-700": {},

      ".choices__item--selectable": {
        "&.is-highlighted": {
          "@apply bg-slate-100 dark:bg-zink-600": {}
        }
      }
    },

    ".is-active": {
      "&.choices__list--dropdown": {
        "@apply visible": {}
      },

      "&.choices__list": {
        "&[aria-expanded]": {
          "@apply visible": {}
        }
      }
    },

    ".is-open": {
      "@apply visible": {},

      ".choices__inner": {
        "@apply border-slate-200 dark:border-zink-500 rounded-tl-md rounded-tr-md":
          {}
      },

      "&.is-flipped": {
        "@apply border-slate-200 dark:border-zink-500 rounded-bl-md rounded-br-md":
          {}
      },

      ".choices__list--dropdown, .choices__list[aria-expanded]": {
        "@apply border-slate-200 dark:border-zink-500": {}
      }
    },

    ".is-flipped": {
      ".choices__list--dropdown, .choices__list[aria-expanded]": {
        "@apply top-auto bottom-full mt-0 rounded-tl-md rounded-tr-md": {},
        marginBottom: "-1px"
      }
    },

    "@media (min-width: 640px)": {
      ".choices__list--dropdown": {
        ".choices__item--selectable": {
          paddingRight: "100px",

          "&::after": {
            "@apply text-xs opacity-0 absolute top-1/2 transform translate-y-1/2 hidden ltr:right-2.5 rtl:left-2.5":
              {},
            content: "attr(data-select-text)"
          }
        }
      },

      ".choices__list[aria-expanded]": {
        ".choices__item--selectable": {
          paddingRight: "100px",

          "&::after": {
            "@apply text-xs opacity-0 absolute top-1/2 transform translate-y-1/2 hidden ltr:right-2.5 rtl:left-2.5":
              {},
            content: "attr(data-select-text)"
          }
        }
      }
    }
  });
});
