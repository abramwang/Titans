const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".leaflet-map": {
      height: "300px",
      "&.leaflet-container": {
        zIndex: 0
      }
    },

    ".gmaps, .gmaps-panaroma": {
      height: "300px",
      borderRadius: "3px",
      "@apply bg-slate-200": {}
    },

    ".gmaps-overlay": {
      "@apply block text-center text-white text-base leading-10 rounded bg-custom-500":
        {},
      padding: "10px 20px"
    },

    ".gmaps-overlay_arrow": {
      "@apply left-1/2 w-0 h-0 absolute": {},
      marginLeft: "-16px",
      "&.above": {
        bottom: "-15px",
        borderLeft: `16px solid transparent`,
        borderRight: `16px solid transparent`,
        borderTop: `16px solid ${theme("colors.custom.500")}`
      },

      "&.below": {
        top: "-15px",
        borderLeft: `16px solid transparent`,
        borderRight: `16px solid transparent`,
        borderBottom: `16px solid ${theme("colors.custom.500")}`
      }
    }
  });
});
