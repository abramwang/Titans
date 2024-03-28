const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    ":root": {
      ".fc": {
        "--fc-border-color": theme("colors.slate.200"),
        "--fc-non-business-color": theme("colors.slate.100"),
        "--fc-button-bg-color": theme("colors.custom.500"),
        "--fc-button-border-color": theme("colors.custom.500"),
        "--fc-button-hover-bg-color": theme("colors.custom.600"),
        "--fc-button-hover-border-color": theme("colors.custom.600"),
        "--fc-button-active-bg-color": theme("colors.custom.600"),
        "--fc-button-active-border-color": theme("colors.custom.600"),
        "--fc-neutral-bg-color": theme("colors.slate.50"),
        "--fc-neutral-text-color": theme("colors.slate.500"),
        "--fc-page-bg-color": theme("colors.white")
      }
    },
    ".fc-h-event .fc-event-main": {
      color: "currentcolor"
    },
    ".fc-daygrid-event-dot": {
      "@apply hidden": {}
    },
    ".fc .fc-toolbar": {
      "@apply flex-wrap gap-4": {}
    },
    ':is([data-mode="dark"]:root)': {
      ".fc": {
        "--fc-border-color": theme("colors.zink.500"),
        "--fc-non-business-color": theme("colors.zink.600"),
        "--fc-neutral-bg-color": theme("colors.zink.600"),
        "--fc-neutral-text-color": theme("colors.zink.200"),
        "--fc-page-bg-color": theme("colors.zink.700")
      }
    }
  });
});
