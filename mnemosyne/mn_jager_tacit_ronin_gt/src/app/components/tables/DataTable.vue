<script setup lang="ts">
import { type PropType, onMounted } from "vue";
import DataTable from "datatables.net-vue3";
import DataTablesCore from "datatables.net";
import { TableHeaderType } from "@/app/components/tables/types";

DataTable.use(DataTablesCore);

defineProps({
  data: {
    type: Array,
    default: () => []
  },
  headerItems: {
    type: Array as PropType<TableHeaderType[]>,
    default: () => []
  },
  thClass: {
    type: String,
    default: ""
  },
  trClass: {
    type: String,
    default: ""
  },
  tdClass: {
    type: String,
    default: ""
  },
  theadClass: {
    type: String,
    default: ""
  }
});

onMounted(() => {
  const dtWrapper = document.querySelector(".dataTables_wrapper");
  if (dtWrapper) {
    dtWrapper.classList.add("dt-tailwindcss");
  }
});
</script>

<template>
  <DataTable
    class="data-table-component w-full text-sm align-middle whitespace-nowrap"
  >
    <thead :class="theadClass">
      <tr>
        <th
          v-for="(item, index) in headerItems"
          :key="'data-table-header-' + index"
          :class="thClass"
        >
          {{ item.title }}
        </th>
      </tr>
    </thead>
    <tbody>
      <tr v-for="item in data" :class="trClass">
        <td :class="tdClass" v-for="temp in item">{{ temp }}</td>
      </tr>
    </tbody>
  </DataTable>
</template>

<style lang="scss">
@import "datatables.net-dt";

// .data-table-component {
//   padding-bottom: 20px !important;
//   border: none !important;
//   padding-top: 20px !important;
// }

// .datatable {
//   .dataTables_length {
//     label select {
//       padding: 0.5rem 0.75rem !important;
//       border-color: rgb(226, 232, 240, 1) !important;
//     }
//   }

//   .dataTables_filter {
//     label input {
//       border-color: rgb(226, 232, 240, 1) !important;
//       padding: 0.5rem 1rem !important;
//       margin-left: 0.5rem !important;
//     }
//   }

//   .dataTables_wrapper .dataTables_paginate .paginate_button:hover {
//     background: #fff !important;
//     color: #000 !important;
//     border-color: #e3e3e3;
//   }

//   .paginate_button {
//     border: 1px solid rgb(226, 232, 240, 1) !important;
//     border-radius: 4px !important;
//   }
// }
</style>
