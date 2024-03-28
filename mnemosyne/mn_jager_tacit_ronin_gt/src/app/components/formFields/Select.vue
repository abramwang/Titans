<script lang="ts" setup>
import { PropType } from "vue";
defineProps({
  items: {
    type: Array as PropType<{ title: string; value: string }[]>,
    default: () => [{ title: "Open this select menu", value: null }]
  },
  rounded: {
    type: Boolean,
    default: false
  },
  disabled: {
    type: Boolean,
    default: false
  },
  selectClass: {
    type: String,
    default: ""
  },
  id: {
    type: String,
    required: true
  }
});
</script>
<template>
  <select
    class="form-select border-slate-200 dark:border-zink-500 focus:outline-none focus:border-custom-500 disabled:bg-slate-100 dark:disabled:bg-zink-600 disabled:border-slate-300 dark:disabled:border-zink-500 dark:disabled:text-zink-200 disabled:text-slate-500 dark:text-zink-100 dark:bg-zink-700 dark:focus:border-custom-800 placeholder:text-slate-400 dark:placeholder:text-zink-200"
    :disabled="disabled"
    :class="`${rounded ? 'rounded-full' : ''} ${selectClass}`"
  >
    <option
      v-for="(item, index) in items"
      :key="'select-' + index"
      :value="item.value"
    >
      {{ item.title }}
    </option>
  </select>
</template>
