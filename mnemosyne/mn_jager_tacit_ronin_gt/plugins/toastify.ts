const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".toastify": {
      "@apply text-white inline-block bg-custom-500 fixed opacity-0 rounded-md cursor-pointer no-underline":
        {},
      padding: "12px 16px",
      maxWidth: "calc(50% - 20px)",
      zIndex: "9999",
      transition: "all 0.4s cubic-bezier(0.215, 0.61, 0.355, 1)",

      "&.on": {
        "@apply opacity-100": {}
      }
    },
    ".toast-close": {
      "@apply opacity-40 relative ltr:left-1 rtl:right-1 ltr:ml-1 rtl:mr-1": {},
      padding: "0 5px"
    },
    ".toastify-right": {
      "@apply ltr:right-4 rtl:left-4": {}
    },
    ".toastify-left": {
      "@apply ltr:left-4 rtl:right-4": {},
      ".toast-close": {
        "@apply ltr:-left-1 rtl:-right-1 ltr:ml-1 rtl:mr-1 ltr:mr-0 rtl:ml-0":
          {}
      }
    },

    ".toastify-top": {
      "@apply -top-40": {}
    },

    ".toastify-bottom": {
      "@apply -bottom-40": {}
    },

    ".toastify-rounded": {
      "@apply rounded-md": {}
    },

    ".toastify-avatar": {
      width: "1.5em",
      height: "1.5em",
      margin: "-7px 5px",
      borderRadius: "2px"
    },

    ".toastify-center": {
      "@apply mx-auto inset-x-0": {},
      maxWidth: "fit-content"
    },

    "@media only screen and (max-width: 360px)": {
      ".toastify-right, .toastify-left": {
        "@apply mx-auto inset-x-0": {},
        maxWidth: "fit-content"
      }
    }
  });
});
