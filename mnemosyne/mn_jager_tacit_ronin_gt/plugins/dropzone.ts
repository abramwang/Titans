const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ".dropzone": {
      minHeight: "200px",
      ".dz-preview": {
        ".dz-image": {
          "@apply overflow-hidden relative flex bg-slate-300 text-center justify-center items-center":
            {},
          borderRadius: "20px",
          width: "120px",
          height: "120px",
          zIndex: 10,

          img: {
            "@apply flex justify-center items-center": {},
            height: "90px",
            width: "90px"
          },

          ".dz-remove": {
            marginTop: "10px",
            "@apply text-xs": {}
          }
        }
      }
    }
  });
});
