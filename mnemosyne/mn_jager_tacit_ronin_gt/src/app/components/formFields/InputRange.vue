<script lang="ts" setup>
import { computed } from "vue";
const props = defineProps({
  min: {
    type: Number,
    default: 0
  },
  max: {
    type: Number,
    default: 100
  },
  step: {
    type: Number,
    default: 1
  },
  color: {
    type: String,
    default: "custom"
  }
});
const getColors = computed(() => {
  switch (props.color) {
    case "custom":
      return "range-custom";
    case "yellow":
      return "range-yellow";
    case "green":
      return "range-green";
    case "red":
      return "range-red";
    case "orange":
      return "range-orange";
    case "purple":
      return "range-purple";
    case "sky":
      return "range-sky";
    case "slate":
      return "range-slate";
    default:
      return "range-custom";
  }
});
</script>
<template>
  <input
    type="range"
    :min="min"
    :max="max"
    :step="step"
    class="w-full h-2 rounded-md appearance-none bg-slate-200 dark:bg-zink-600"
    :class="`${getColors}`"
  />
</template>
