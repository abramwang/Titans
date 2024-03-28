<script lang="ts" setup>
import { ref } from "vue";
import { tableData, headerItems } from "@/components/tables/listJs/utils";

const isAllSelect = ref(false);
const mapData = tableData.map((item) => {
  return {
    ...item,
    checked: false
  };
});
const finalData = ref(mapData);
const onSelectAll = () => {
  isAllSelect.value = !isAllSelect.value;
  finalData.value = finalData.value.map((item) => {
    return {
      ...item,
      checked: isAllSelect.value
    };
  });
};
</script>
<template>
  <TCard class="overflow-x-auto">
    <TListJsTable
      id="customer-table"
      :items="finalData"
      :headerItems="headerItems"
      theadClass="bg-slate-100 dark:bg-zink-600"
      tdClass="px-3.5 py-2.5 border-y border-slate-200 dark:border-zink-500 id"
      tbodyClass="form-check-all"
      @onSelectAll="onSelectAll"
    >
      <template #checked>
        <input
          class="size-4 border rounded-sm appearance-none cursor-pointer bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-custom-500 checked:border-custom-500 dark:checked:bg-custom-500 dark:checked:border-custom-500 checked:disabled:bg-custom-400 checked:disabled:border-custom-400"
          type="checkbox"
          value="option"
          name="chk_child"
          :checked="isAllSelect"
        />
      </template>

      <template #status="{ value }">
        <TLabel :color="value.status === 'Active' ? 'green' : 'red'">
          {{ value.status }}
        </TLabel>
      </template>
      <template #action="{ value }">
        <div class="flex gap-2">
          <TButton classes="py-1 text-xs edit-item-btn">Edit</TButton>
          <TButton
            classes="py-1 text-xs delete-btn"
            color="red"
            :id="'delete-record-' + value.id"
          >
            Remove
          </TButton>
        </div>
      </template>
    </TListJsTable>
  </TCard>
</template>
