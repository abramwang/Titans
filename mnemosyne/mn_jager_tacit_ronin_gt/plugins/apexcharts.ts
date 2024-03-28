const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".apex-charts": {
      minHeight: "auto !important",

      text: {
        "@apply !font-public dark:fill-zink-50": {}
      },

      ".apexcharts-canvas": {
        "@apply my-0 mx-auto": {}
      }
    },
    ".apexcharts-canvas": {
      "::-webkit-scrollbar-thumb": {
        "@apply bg-slate-600 shadow-md dark:bg-zink-600": {}
      },

      ":is(.apexcharts-reset-zoom-icon, .apexcharts-selection-icon, .apexcharts-zoom-icon)":
        {
          "&.apexcharts-selected": {
            svg: {
              "@apply fill-custom-500": {}
            }
          }
        },

      "text.apexcharts-title-text, .apexcharts-subtitle-text": {
        "@apply !fill-slate-800 !font-medium !font-public dark:!fill-zink-100":
          {}
      }
    },

    ".apexcharts-gridline": {
      "@apply stroke-slate-200 dark:stroke-zink-500": {}
    },

    ".apexcharts-yaxis, .apexcharts-xaxis": {
      text: {
        "@apply fill-slate-500 font-public dark:fill-zink-200": {}
      }
    },

    ".apexcharts-heatmap-series rect, .apexcharts-treemap-series rect": {
      "@apply stroke-white dark:!stroke-zink-700": {}
    },

    ".apexcharts-legend-text": {
      "@apply !text-slate-800 !font-public !text-base dark:!text-zink-100": {}
    },

    ".apexcharts-xaxis-tick": {
      "@apply stroke-slate-200 dark:stroke-zink-500": {}
    },
    ".apexcharts-marker": {
      "@apply stroke-white dark:stroke-zink-700": {}
    },

    ".apexcharts-tooltip": {
      "@apply shadow-lg": {},

      "&.apexcharts-theme-light": {
        "@apply border-slate-200 bg-white dark:!bg-zink-600 dark:!border-zink-500":
          {},

        ".apexcharts-tooltip-title": {
          "@apply !border-b-slate-200 !bg-white !font-public dark:!border-b-zink-500 dark:!bg-zink-600":
            {}
        }
      }
    },

    ".apexcharts-tooltip-title": {
      "@apply !font-public": {}
    },

    ".apexcharts-heatmap-series, .apexcharts-treemap-series": {
      rect: {
        "@apply stroke-white": {}
      }
    },

    ".apexcharts-pie-series, .apexcharts-bar-series": {
      path: {
        "@apply stroke-white": {}
      }
    },

    ".apexcharts-boxPlot-series": {
      path: {
        "@apply stroke-slate-200": {}
      }
    },
    ".apexcharts-radialbar": {
      ".apexcharts-datalabels-group text": {
        "@apply fill-slate-800 dark:fill-zink-50": {}
      }
    },

    ".apexcharts-radialbar-track": {
      path: {
        "@apply stroke-slate-100 dark:stroke-zink-600": {}
      }
    },

    ".apexcharts-radar-series": {
      "polygon, line": {
        "@apply stroke-slate-200 dark:stroke-zink-500": {}
      }
    },

    ".apexcharts-pie": {
      "circle, line": {
        "@apply stroke-slate-200 dark:stroke-zink-500": {},

        '&[fill="transparent"]': {
          "@apply stroke-transparent": {}
        }
      },
      text: {
        "@apply fill-white": {}
      }
    },
    ".apexcharts-xaxistooltip": {
      "&.apexcharts-theme-light": {
        "@apply shadow-md text-slate-800 dark:text-zink-50 bg-white border-slate-200 dark:border-zink-500 dark:bg-zink-700 !font-public":
          {},

        "&::before": {
          "@apply border-b-slate-200 dark:border-b-zink-600": {}
        }
      }
    },
    ".apexcharts-grid-borders": {
      line: {
        "@apply stroke-slate-200 dark:stroke-zink-500": {}
      }
    },
    "#dynamicloadedchart-wrap": {
      "@apply relative my-0 mx-auto max-w-4xl": {}
    },

    ".chart-box": {
      "@apply p-0": {}
    },
    ":is(#chart-year, #chart-quarter)": {
      "@apply stroke-slate-200 dark:stroke-zink-500 w-[96%] max-w-[48%] shadow-none pl-0 pt-5 bg-white border border-slate-200 dark:border-zink-500 dark:bg-zink-700 float-left relative":
        {},
      transition: "1s ease transform"
    },
    "#chart-year": {
      "@apply z-[3]": {},
      "&.chart-quarter-activated": {
        transform: "translateX(0)",
        transition: "1s ease transform"
      }
    },

    "#chart-quarter": {
      "@apply z-[-2]": {},
      "&.active": {
        transition: "1.1s ease-in-out transform",
        transform: "translateX(0)",
        "@apply z-[1]": {}
      }
    },

    "@media screen and (min-width: 480px)": {
      "#chart-year": {
        transform: "translateX(50%)"
      },
      "#chart-quarter": {
        transform: "translateX(-50%)"
      }
    },

    ".apexcharts-boxPlot-series path": {
      "@apply stroke-slate-200 dark:stroke-zink-500": {}
    },

    ".apexcharts-pie-series path, .apexcharts-bar-series path": {
      "@apply stroke-white dark:stroke-zink-700": {}
    },

    ".apexcharts-menu": {
      "@apply bg-white dark:bg-zink-600 border-slate-200 dark:border-zink-500":
        {},
      "apexcharts-menu-item:hover": {
        "@apply !bg-zink-500": {}
      }
    },

    "#totalProjectChart": {
      ".apexcharts-legend-series": {
        "@apply px-2 py-1 border rounded-sm border-slate-200 dark:border-zink-500":
          {}
      }
    }
  });
});
