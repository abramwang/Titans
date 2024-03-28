<script lang="ts" setup>
import { PropType } from "vue";
import { computed } from "vue";

const props = defineProps({
  title: {
    type: String,
    default: ""
  },
  variant: {
    type: String as PropType<"bordered" | "dashed-bordered" | "fill" | "soft">,
    default: ""
  },
  color: {
    type: String,
    default: ""
  },
  noBody: {
    type: Boolean,
    default: false
  },
  bodyClass: {
    type: String,
    default: ""
  }
});

const titleClass = computed(() => {
  if (props.variant === "fill") {
    return "text-white";
  } else if (props.variant === "soft") {
    switch (props.color) {
      case "yellow":
        return "text-yellow-500 dark:text-yellow-200";
      case "green":
        return "text-green-500 dark:text-green-200";
      case "red":
        return "text-red-500 dark:text-red-200";
      case "orange":
        return "text-orange-500 dark:text-orange-200";
      case "purple":
        return "text-purple-500 dark:text-purple-200";
      case "sky":
        return "text-sky-500 dark:text-sky-200";
      case "slate":
        return "text-slate-500 dark:text-slate-200";
      default:
        return "text-custom-500 dark:text-custom-200";
    }
  }
});

const cardClass = computed(() => {
  if (props.variant === "bordered" || props.variant === "dashed-bordered") {
    switch (props.color) {
      case "yellow":
        return "border-yellow-200 dark:border-yellow-800";
      case "green":
        return "border-green-200 dark:border-green-800";
      case "red":
        return "border-red-200 dark:border-red-800";
      case "orange":
        return "border-orange-200 dark:border-orange-800";
      case "purple":
        return "border-purple-200 dark:border-purple-800";
      case "sky":
        return "border-sky-200 dark:border-sky-800";
      case "slate":
        return "border-slate-200 dark:border-slate-800";
      default:
        return "border-custom-200 dark:border-custom-800";
    }
  } else if (props.variant === "fill") {
    switch (props.color) {
      case "yellow":
        return "bg-yellow-500 border-yellow-500 dark:bg-yellow-800 dark:border-yellow-800";
      case "green":
        return "bg-green-500 border-green-500 dark:bg-green-800 dark:border-green-800";
      case "red":
        return "bg-red-500 border-red-500 dark:bg-red-800 dark:border-red-800";
      case "orange":
        return "bg-orange-500 border-orange-500 dark:bg-orange-800 dark:border-orange-800";
      case "purple":
        return "bg-purple-500 border-purple-500 dark:bg-purple-800 dark:border-purple-800";
      case "sky":
        return "bg-sky-500 border-sky-500 dark:bg-sky-800 dark:border-sky-800";
      case "slate":
        return "bg-slate-500 border-slate-500 dark:bg-slate-800 dark:border-slate-800";
      default:
        return "bg-custom-500 border-custom-500 dark:bg-custom-800 dark:border-custom-800";
    }
  } else if (props.variant === "soft") {
    switch (props.color) {
      case "yellow":
        return "bg-yellow-100 border-yellow-200 dark:bg-yellow-400/20 dark:border-yellow-600";
      case "green":
        return "bg-green-100 border-green-200 dark:bg-green-400/20 dark:border-green-600";
      case "red":
        return "bg-red-100 border-red-200 dark:bg-red-400/20 dark:border-red-600";
      case "orange":
        return "bg-orange-100 border-orange-200 dark:bg-orange-400/20 dark:border-orange-600";
      case "purple":
        return "bg-purple-100 border-purple-200 dark:bg-purple-400/20 dark:border-purple-600";
      case "sky":
        return "bg-sky-100 border-sky-200 dark:bg-sky-400/20 dark:border-sky-600";
      case "slate":
        return "bg-slate-100 border-slate-200 dark:bg-slate-400/20 dark:border-slate-600";
      default:
        return "bg-custom-100 border-custom-200 dark:bg-custom-400/20 dark:border-custom-600";
    }
  }
  return "";
});

const contentClass = computed(() => {
  if (props.variant === "fill") {
    switch (props.color) {
      case "yellow":
        return "dark:text-yellow-300 text-yellow-200";
      case "green":
        return "dark:text-green-300 text-green-200";
      case "red":
        return "dark:text-red-300 text-red-200";
      case "orange":
        return "dark:text-orange-300 text-orange-200";
      case "purple":
        return "dark:text-purple-300 text-purple-200";
      case "sky":
        return "dark:text-sky-300 text-sky-200";
      case "slate":
        return "dark:text-slate-300 text-slate-200";
      default:
        return "dark:text-custom-300 text-custom-200";
    }
  } else if (props.variant === "soft") {
    switch (props.color) {
      case "yellow":
        return "text-yellow-400";
      case "green":
        return "text-green-400";
      case "red":
        return "text-red-400";
      case "orange":
        return "text-orange-400";
      case "purple":
        return "text-purple-400";
      case "sky":
        return "text-sky-400";
      case "slate":
        return "text-slate-400";
      default:
        return "text-custom-400";
    }
  }
});
</script>
<template>
  <div
    class="card"
    :class="`${cardClass} 
        ${variant === 'dashed-bordered' ? 'border border-dashed' : ''}
         ${variant === 'bordered' ? 'border' : ''}`"
  >
    <slot name="header" />
    <div :class="`${!noBody ? 'card-body' : ''} ${bodyClass}`">
      <div :class="title ? 'mb-4' : ''">
        <!-- inline-block -->
        <div class="flex flex-wrap justify-between">
          <h6 v-if="title" class="text-15 mb-4 lg:mb-0" :class="titleClass">
            {{ title }}
          </h6>
          <slot name="titleAction" />
        </div>
        <slot name="subtitle" />
      </div>
      <div :class="contentClass">
        <slot />
      </div>
    </div>
    <slot name="action" />
  </div>
</template>
