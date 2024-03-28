const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    "[data-simplebar]": {
      "@apply relative flex-col flex-wrap ltr:content-start ltr:items-start rtl:content-end rtl:items-end":
        {}
    },
    ".simplebar-wrapper": {
      "@apply overflow-hidden": {},
      width: "inherit",
      height: "inherit",
      maxWidth: "inherit",
      maxHeight: "inherit"
    },
    ".simplebar-mask": {
      direction: "inherit",
      "@apply absolute overflow-hidden p-0 m-0 inset-0 !w-auto !h-auto z-0": {}
    },
    ".simplebar-offset": {
      direction: "inherit !important",
      boxSizing: "inherit !important",
      "@apply !resize-none absolute inset-y-0 !inset-x-0 p-0 m-0": {},
      WebkitOverflowScrolling: "touch"
    },
    ".simplebar-content-wrapper": {
      direction: "inherit",
      "@apply !box-border relative block h-full w-auto visible overflow-auto max-w-full max-h-full !p-0":
        {},
      scrollbarWidth: "none"
    },
    ".simplebar-content-wrapper::-webkit-scrollbar, .simplebar-hide-scrollbar::-webkit-scrollbar":
      {
        "@apply hidden": {}
      },
    ".simplebar-content::before, .simplebar-content::after": {
      content: `' '`,
      "@apply table": {}
    },
    ".simplebar-placeholder": {
      "@apply max-w-full max-h-full w-full pointer-events-none": {}
    },
    ".simplebar-height-auto-observer-wrapper": {
      boxSizing: "inherit !important",
      "@apply w-full h-full relative ltr:float-left rtl:float-right overflow-hidden p-0 m-0 pointer-events-none shrink-0 basis-0":
        {},
      maxWidth: "1px",
      maxHeight: "1px",
      zIndex: -1,
      flexGrow: "inherit"
    },
    ".simplebar-height-auto-observer": {
      boxSizing: "inherit",
      "@apply opacity-0 block absolute top-0 ltr:left-0 rtl:right-0 overflow-hidden pointer-events-none":
        {},
      height: "1000%",
      width: "1000%",
      minHeight: "1px",
      minWidth: "1px",
      zIndex: -1
    },
    ".simplebar-track": {
      "@apply absolute ltr:right-0 rtl:left-0 bottom-0 overflow-hidden pointer-events-none":
        {},
      zIndex: 1
    },
    "[data-simplebar].simplebar-dragging .simplebar-content": {
      "@apply pointer-events-none select-none": {}
    },
    "[data-simplebar].simplebar-dragging .simplebar-track": {
      pointerEvents: "all"
    },
    ".simplebar-scrollbar": {
      "@apply absolute ltr:right-px rtl:left-px w-1.5 before:absolute before:bg-slate-500 before:rounded-md before:left-0 before:right-0 before:opacity-0":
        {},
      minHeight: "10px"
    },
    ".simplebar-scrollbar:before": {
      content: `' '`,
      transition: "opacity 0.2s linear"
    },
    ".simplebar-scrollbar.simplebar-visible:before": {
      opacity: theme("opacity.50"),
      transition: "opacity 0s linear"
    },
    ".simplebar-track.simplebar-vertical": {
      "@apply top-0 w-2.5": {}
    },
    ".simplebar-track.simplebar-vertical .simplebar-scrollbar:before": {
      "@apply top-0.5 bottom-0.5": {}
    },
    ".simplebar-track.simplebar-horizontal": {
      "@apply ltr:left-0 rtl:right-0 h-3": {}
    },
    ".simplebar-track.simplebar-horizontal .simplebar-scrollbar:before": {
      "@apply h-full left-0.5 right-0.5": {}
    },
    ".simplebar-track.simplebar-horizontal .simplebar-scrollbar": {
      "@apply ltr:right-auto rtl:left-auto ltr:left-0 rtl:right-0 top-0.5 h-2 min-h-0 w-auto":
        {},
      minWidth: "10px"
    },
    '[data-simplebar-direction="rtl"] .simplebar-track.simplebar-vertical': {
      "@apply ltr:right-auto rtl:left-auto ltr:left-0 rtl:right-0": {}
    },
    ".hs-dummy-scrollbar-size": {
      direction: "rtl",
      "@apply fixed opacity-0 invisible overflow-x-scroll overflow-y-auto": {},
      height: "500px",
      width: "500px"
    },
    ".simplebar-hide-scrollbar": {
      "@apply fixed ltr:left-0 rtl:right-0 invisible overflow-y-scroll": {},
      scrollbarWidth: "none"
    },
    ".custom-scroll": {
      "@apply h-full": {}
    },
    '[data-simplebar-track="custom"]': {
      ".simplebar-scrollbar:before": {
        "@apply bg-custom-500": {}
      }
    },
    '[data-simplebar-track="green"]': {
      ".simplebar-scrollbar:before": {
        "@apply bg-green-500": {}
      }
    },
    '[data-simplebar-track="yellow"]': {
      ".simplebar-scrollbar:before": {
        "@apply bg-yellow-500": {}
      }
    },
    '[data-simplebar-track="orange"]': {
      ".simplebar-scrollbar:before": {
        "@apply bg-orange-500": {}
      }
    },
    '[data-simplebar-track="sky"]': {
      ".simplebar-scrollbar:before": {
        "@apply bg-sky-500": {}
      }
    },
    '[data-simplebar-track="red"]': {
      ".simplebar-scrollbar:before": {
        "@apply bg-red-500": {}
      }
    }
  });
});
