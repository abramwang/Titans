<script lang="ts" setup>
import { computed } from "vue";

const props = defineProps({
  src: {
    type: String,
    default: ""
  },
  height: {
    type: Number,
    default: 8
  },
  width: {
    type: Number,
    default: null
  },
  rounded: {
    type: Boolean,
    default: false
  },
  class: {
    type: String,
    default: ""
  },
  bordered: {
    type: Boolean,
    default: false
  },
  color: {
    type: String,
    default: ""
  },
  badge: {
    type: Boolean,
    default: false
  },
  redBadge: {
    type: Boolean,
    default: false
  },
  badgeClass: {
    type: String,
    default: ""
  }
});

const getHeightClass = computed(() => {
  switch (props.height) {
    case 10:
      return "h-10";
    case 12:
      return "h-12";
    case 14:
      return "h-14";
    case 16:
      return "h-16";
    case 20:
      return "h-20";
    case 24:
      return "h-24";
    case 28:
      return "h-28";
    default:
      return "h-8";
  }
});

const getWidthClass = computed(() => {
  switch (props.width) {
    case 10:
      return "w-10";
    case 12:
      return "w-12";
    case 14:
      return "w-14";
    case 16:
      return "w-16";
    case 20:
      return "w-20";
    case 24:
      return "w-24";
    case 28:
      return "w-28";
    default:
      return "w-8";
  }
});

const getColorClass = computed(() => {
  switch (props.color) {
    case "custom":
      return "bg-custom-100 text-custom-500 dark:bg-custom-950";
    case "yellow":
      return "bg-yellow-100 text-yellow-500 dark:bg-yellow-950";
    case "green":
      return "bg-green-100 text-green-500 dark:bg-green-950";
    case "red":
      return "bg-red-100 text-red-500 dark:bg-red-950";
    case "orange":
      return "bg-orange-100 text-orange-500 dark:bg-orange-950";
    case "purple":
      return "bg-purple-100 text-purple-500 dark:bg-purple-950";
    case "sky":
      return "bg-sky-100 text-sky-500 dark:bg-sky-950";
    case "slate":
      return "bg-slate-100 text-slate-500 dark:bg-slate-950";
  }
});

const getClass = computed(() => {
  let classes = `${props.class} ${getHeightClass.value} ${getColorClass.value}`;

  if (props.rounded) {
    classes = `${classes} rounded-full`;
  } else {
    classes = `${classes} rounded-md`;
  }

  if (props.width) {
    classes = `${classes} ${getWidthClass.value}`;
  }

  if (props.bordered) {
    classes = `${classes} ring-1 ring-offset-2 ring-slate-200 dark:ring-offset-zink-700 dark:ring-zink-500`;
  }

  return classes;
});

const getBadgeClasses = computed(() => {
  switch (props.height) {
    case 12:
      return props.rounded
        ? "top-0 size-3 ltr:right-0 rtl:left-0"
        : "size-3 -top-1 ltr:-right-1 rtl:-left-1";
    case 14:
      return props.rounded
        ? "top-0 size-3 ltr:right-0 rtl:left-0"
        : "size-3 -top-1 ltr:-right-1 rtl:-left-1";
    case 16:
      return props.rounded
        ? "w-3.5 h-3.5 top-0.5 ltr:right-0.5 rtl:left-0.5"
        : "w-3.5 h-3.5 -top-1.5 ltr:-right-1.5 rtl:-left-1.5";
    case 20:
      return props.rounded
        ? "size-4 top-0.5 ltr:right-0.5 rtl:left-0.5"
        : "size-4 -top-1.5 ltr:-right-1.5 rtl:-left-1.5";
    case 24:
      return props.rounded
        ? "size-5 top-0.5 ltr:right-0.5 rtl:left-0.5"
        : "size-5 -top-2 ltr:-right-2 rtl:-left-2";
    case 28:
      return props.rounded
        ? "size-6 top-1 ltr:right-1 rtl:left-1"
        : "size-6 -top-2.5 ltr:-right-2.5 rtl:-left-2.5";
    default:
      return props.rounded
        ? "top-0 ltr:right-0 rtl:left-0 w-2.5 h-2.5"
        : "-top-1 ltr:-right-1 rtl:-left-1 w-2.5 h-2.5";
  }
});

const getBadeColor = computed(() => {
  if (props.redBadge) {
    return "bg-red-400";
  }

  return "bg-green-400";
});
</script>
<template>
  <div class="relative">
    <img v-if="src" :src="src" :class="getClass" />
    <div
      v-else
      :class="getClass"
      class="flex justify-center items-center overflow-hidden"
    >
      <slot />
    </div>
    <span
      v-if="badge"
      :class="`absolute border-2 border-white rounded-full dark:border-zink-700 ${getBadgeClasses} ${getBadeColor} ${badgeClass}`"
    />
  </div>
</template>
