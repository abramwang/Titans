const plugin = require("tailwindcss/plugin");

module.exports = plugin(function ({ addComponents, theme }) {
  addComponents({
    table: {
      "&.dataTable": {
        td: {
          "&.dt-control": {
            "@apply text-center cursor-pointer": {},

            "&::before": {
              "@apply inline-block text-slate-500": {},
              content: `"►"`
            }
          }
        },

        tr: {
          "&.dt-hasChild": {
            "td.dt-control:before": {
              content: `"▼"`
            }
          }
        },

        "thead>tr>th.sorting, thead>tr>th.sorting_asc, thead>tr>th.sorting_desc, thead>tr>th.sorting_asc_disabled, thead>tr>th.sorting_desc_disabled, thead>tr>td.sorting, thead>tr>td.sorting_asc, thead>tr>td.sorting_desc, thead>tr>td.sorting_asc_disabled, thead>tr>td.sorting_desc_disabled":
          {
            "@apply cursor-pointer relative pr-6": {}
          },

        ":is(thead>tr>th, thead>tr>td)": {
          "&.sorting:before, &.sorting:after, &.sorting_asc:before, &.sorting_asc:after, &.sorting_desc:before, &.sorting_desc:after, &.sorting_asc_disabled:before, &.sorting_asc_disabled:after, &.sorting_desc_disabled:before, &.sorting_desc_disabled:after":
            {
              "@apply absolute block text-base": {},
              opacity: ".125",
              right: "10px",
              lineHeight: "11px"
            },

          "&.sorting:before, &.sorting_asc:before, &.sorting_desc:before, &.sorting_asc_disabled:before, &.sorting_desc_disabled:before":
            {
              bottom: "50%",
            //   content: `"▲"`,
              content: `"▲" /""`
            },
          "&.sorting:after, &.sorting_asc:after, &.sorting_desc:after, &.sorting_asc_disabled:after, &.sorting_desc_disabled:after":
            {
              top: "50%",
            //   content: `"▼"`,
              content: `"▼" /""`
            },
          "&.sorting_asc:before, &.sorting_desc:after ": {
            "@apply opacity-60": {}
          },
          "&.sorting_desc_disabled:after, &.sorting_asc_disabled:before": {
            "@apply hidden": {}
          }
        },

        ".dataTables_empty": {
          "@apply p-6 text-center text-lg": {}
        }
      }
    },
    "div.dataTables_scrollBody>table.dataTable": {
      ">:is(thead>tr>th, thead>tr>td)": {
        "&::after, &:before": {
          "@apply hidden": {}
        }
      }
    },
    "table.dataTable.no-footer, table.dataTable > thead > tr > th, table.dataTable > thead > tr > td": {
      "@apply !border-b-0": {}
    },
    ".dataTables_wrapper .dataTables_paginate .paginate_button.current, .dataTables_wrapper .dataTables_paginate .paginate_button.current:hover": {
      "@apply !border !border-slate-200 dark:!border-zink-500 !bg-gradient-to-r from-white to-white dark:from-zink-700 dark:to-zink-700": {}
    },
    ".dataTables_wrapper .dataTables_paginate .paginate_button:hover" : {
      "@apply !bg-gradient-to-r from-custom-500 to-custom-500 !border !border-custom-500 !text-white": {}
    },
    ".dataTables_wrapper .dataTables_paginate .paginate_button.disabled, .dataTables_wrapper .dataTables_paginate .paginate_button.disabled:hover, .dataTables_wrapper .dataTables_paginate .paginate_button.disabled:active": {
      "@apply !text-slate-300": {}
    },
    ".dataTables_wrapper .dataTables_filter input, .dataTables_wrapper .dataTables_length select": {
      "@apply !border !border-slate-200 dark:!border-zink-500 !outline-none": {}
    }
  });
});
