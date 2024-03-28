<script lang="ts" setup>
import { computed } from "vue";

const props = defineProps({
  color: {
    type: String,
    default: "custom"
  },
  width: {
    type: Number,
    default: 25
  },
  label: {
    type: Boolean,
    default: false
  },
  animate: {
    type: Boolean,
    default: false
  },
  height: {
    type: Number,
    default: 2.5
  }
});

const progressBarClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "bg-yellow-500";
    case "green":
      return "bg-green-500";
    case "red":
      return "bg-red-500";
    case "orange":
      return "bg-orange-500";
    case "purple":
      return "bg-purple-500";
    case "sky":
      return "bg-sky-500";
    case "slate":
      return "bg-slate-500";
    default:
      return "bg-custom-500";
  }
});

const progressBarLabelClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "bg-yellow-500 after:border-t-yellow-500";
    case "green":
      return "bg-green-500 after:border-t-green-500";
    case "red":
      return "bg-red-500 after:border-t-red-500";
    case "orange":
      return "bg-orange-500 after:border-t-orange-500";
    case "purple":
      return "bg-purple-500 after:border-t-purple-500";
    case "sky":
      return "bg-sky-500 after:border-t-sky-500";
    case "slate":
      return "bg-slate-500 after:border-t-slate-500";
    default:
      return "bg-custom-500 after:border-t-custom-500";
  }
});

const progressBarStyle = computed(() => {
  return `width: ${props.width}%`;
});
</script>
<template>
  <div :class="`w-full bg-slate-200 rounded-full dark:bg-zink-600 h-${height}`">
    <div
      :class="`${progressBarClass} ${
        animate ? 'animate-progress' : ''
      } h-${height}`"
      class="rounded-full relative text-white flex justify-center items-center"
      :style="progressBarStyle"
    >
      <div
        v-if="label"
        class="absolute ltr:right-0 rtl:left-0 inline-block px-2 py-0.5 text-[10px] text-white rounded -top-6 after:absolute after:border-4 ltr:after:right-1/2 rtl:after:left-1/2 after:-bottom-2 after:border-transparent"
        :class="progressBarLabelClass"
      >
        {{ width }}%
      </div>
      <slot name="inside" />
    </div>
  </div>
</template>
