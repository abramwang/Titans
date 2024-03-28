const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents }) {
  addComponents({
    ".card": {
      "@apply shadow-md rounded-md shadow-slate-200 border-0 mb-5 border-transparent bg-white dark:bg-zink-700 dark:!shadow-zink-500/20 group-data-[skin=bordered]:shadow-sm group-data-[skin=bordered]:border group-data-[skin=bordered]:border-slate-200 group-data-[skin=bordered]:shadow-lg group-data-[skin=bordered]:shadow-slate-100 group-data-[skin=bordered]:dark:shadow-zink-500/30 group-data-[skin=bordered]:border group-data-[skin=bordered]:dark:border-zink-500":
        {},

      ".card-body": {
        "@apply p-5": {}
      }
    }
  });
});
