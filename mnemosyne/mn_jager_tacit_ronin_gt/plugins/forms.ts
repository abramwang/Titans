const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    '[type="checkbox"]:checked': {
      "@apply bg-no-repeat": {},
      backgroundImage: 'url("../images/check-arrow.svg")',
      backgroundSize: ".9rem .9rem",
      backgroundPosition: "50%"
    },
    '[type="radio"]:checked': {
      "@apply bg-no-repeat": {},
      backgroundImage: 'url("../images/radio-arrow.svg")',
      backgroundSize: ".75rem .75rem",
      backgroundPosition: "58%"
    },
    '[type="checkbox"].arrow-none:checked, [type="radio"].arrow-none:checked': {
      "@apply bg-no-repeat": {}
    },

    ".form-input": {
      "@apply border rounded-md block text-base py-2 px-4 w-full": {}
    },
    ".form-select": {
      "@apply border rounded-md block  py-2 pl-3 pr-10 px-4 text-base w-full bg-no-repeat appearance-none":
        {},
      backgroundImage: `url("data:image/svg+xml,%3csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 16 16'%3e%3cpath fill='none' stroke='%231f242e' stroke-linecap='round' stroke-linejoin='round' stroke-width='2' d='m2 5 6 6 6-6'/%3e%3c/svg%3e")`,
      backgroundPosition: "right 0.70rem center",
      backgroundRepeat: "no-repeat",
      backgroundSize: "0.70em 0.70em"
    },
    //range input element
    'input[type="range"]': {
      "&::-webkit-slider-thumb": {
        "@apply bg-custom-500 h-4 w-4 rounded-full cursor-pointer appearance-none":
          {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.custom.700")}`
      }
    },
    'input[type="range"].range-green': {
      "&::-webkit-slider-thumb": {
        "@apply bg-green-500": {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.green.700")}`
      }
    },
    'input[type="range"].range-orange': {
      "&::-webkit-slider-thumb": {
        "@apply bg-orange-500": {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.orange.700")}`
      }
    },
    'input[type="range"].range-sky': {
      "&::-webkit-slider-thumb": {
        "@apply bg-sky-500": {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.sky.700")}`
      }
    },
    'input[type="range"].range-yellow': {
      "&::-webkit-slider-thumb": {
        "@apply bg-yellow-500": {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.yellow.700")}`
      }
    },
    'input[type="range"].range-red': {
      "&::-webkit-slider-thumb": {
        "@apply bg-red-500": {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.red.700")}`
      }
    },
    'input[type="range"].range-purple': {
      "&::-webkit-slider-thumb": {
        "@apply bg-purple-500": {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.purple.700")}`
      }
    },
    'input[type="range"].range-dark': {
      "&::-webkit-slider-thumb": {
        "@apply bg-slate-600": {}
      },
      "&:focus::-webkit-slider-thumb": {
        boxShadow: `0 0 3px ${theme("colors.slate.700")}`
      }
    },
    //file upload elements
    ".form-file": {
      "@apply border rounded-md block text-base w-full": {},
      "&::-webkit-file-upload-button": {
        "@apply py-2 px-4 text-slate-500 bg-slate-100 dark:text-zink-200 dark:bg-zink-600 cursor-pointer border-none shadow-none outline-none":
          {}
      },
      "&.form-file-sm": {
        "&::-webkit-file-upload-button": {
          "@apply py-1 px-3": {}
        }
      },
      "&.form-file-lg": {
        "&::-webkit-file-upload-button": {
          "@apply py-3 px-5": {}
        }
      }
    }
  });
});
