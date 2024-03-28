<script lang="ts" setup>
import { computed } from "vue";
import { X } from "lucide-vue-next";

const props = defineProps({
  color: { type: String, default: "custom" },
  outlined: { type: Boolean, default: false },
  bordered: { type: Boolean, default: false },
  dismissible: { type: Boolean, default: false },
  borderTop: { type: Boolean, default: false },
  solid: { type: Boolean, default: false }
});

const defaultClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "text-yellow-500 bg-yellow-50 dark:bg-yellow-400/20";
    case "green":
      return "text-green-500 bg-green-50 dark:bg-green-400/20";
    case "red":
      return "text-red-500 bg-red-50 dark:bg-red-400/20";
    case "orange":
      return "text-orange-500 bg-orange-50 dark:bg-orange-400/20";
    case "purple":
      return "text-purple-500 bg-purple-50 dark:bg-purple-400/20";
    case "sky":
      return "text-sky-500 bg-sky-50 dark:bg-sky-400/20";
    case "slate":
      return "text-slate-500 bg-slate-50 dark:bg-slate-400/20";
    default:
      return "text-custom-500 bg-custom-50 dark:bg-custom-400/20";
  }
});

const outlinedClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "border-yellow-300 text-yellow-500 dark:bg-yellow-400/20";
    case "green":
      return "border-green-300 text-green-500 dark:bg-green-400/20";
    case "red":
      return "border-red-300 text-red-500 dark:bg-red-400/20";
    case "orange":
      return "border-orange-300 text-orange-500 dark:bg-orange-400/20";
    case "purple":
      return "border-purple-300 text-purple-500 dark:bg-purple-400/20";
    case "sky":
      return "border-sky-300 text-sky-500 dark:bg-sky-400/20";
    case "slate":
      return "border-slate-300 text-slate-500 dark:bg-slate-400/20";
    default:
      return "border-custom-300 text-custom-500 dark:bg-custom-400/20";
  }
});

const borderedClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "border-yellow-300 text-yellow-500 dark:bg-yellow-400/20 bg-yellow-50";
    case "green":
      return "border-green-300 text-green-500 dark:bg-green-400/20 bg-green-50";
    case "red":
      return "border-red-300 text-red-500 dark:bg-red-400/20 bg-red-50";
    case "orange":
      return "border-orange-300 text-orange-500 dark:bg-orange-400/20 bg-orange-50";
    case "purple":
      return "border-purple-300 text-purple-500 dark:bg-purple-400/20 bg-purple-50";
    case "sky":
      return "border-sky-300 text-sky-500 dark:bg-sky-400/20 bg-sky-50";
    case "slate":
      return "border-slate-300 text-slate-500 dark:bg-slate-400/20 bg-slate-50";
    default:
      return "border-custom-300 text-custom-500 dark:bg-custom-400/20 bg-custom-50";
  }
});

const topBorderClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "border-yellow-300 text-yellow-500 dark:bg-yellow-400/20 bg-yellow-50";
    case "green":
      return "border-green-300 text-green-500 dark:bg-green-400/20 bg-green-50";
    case "red":
      return "border-red-300 text-red-500 dark:bg-red-400/20 bg-red-50";
    case "orange":
      return "border-orange-300 text-orange-500 dark:bg-orange-400/20 bg-orange-50";
    case "purple":
      return "border-purple-300 text-purple-500 dark:bg-purple-400/20 bg-purple-50";
    case "sky":
      return "border-sky-300 text-sky-500 dark:bg-sky-400/20 bg-sky-50";
    case "slate":
      return "border-slate-300 text-slate-500 dark:bg-slate-400/20 bg-slate-50";
    default:
      return "border-custom-300 text-custom-500 dark:bg-custom-400/20 bg-custom-50";
  }
});

const solidClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "text-yellow-50 bg-yellow-500";
    case "green":
      return "text-green-50 bg-green-500";
    case "red":
      return "text-red-50 bg-red-500";
    case "orange":
      return "text-orange-50 bg-orange-500";
    case "purple":
      return "text-purple-50 bg-purple-500";
    case "sky":
      return "text-sky-50 bg-sky-500";
    case "slate":
      return "text-slate-50 bg-slate-500";
    default:
      return "text-custom-50 bg-custom-500";
  }
});

const dismissibleClass = computed(() => {
  switch (props.color) {
    case "yellow":
      return "text-yellow-200 hover:text-yellow-500 dark:text-yellow-400/50 dark:hover:text-yellow-500";
    case "green":
      return "text-green-200 hover:text-green-500 dark:text-green-400/50 dark:hover:text-green-500";
    case "red":
      return "text-red-200 hover:text-red-500 dark:text-red-400/50 dark:hover:text-red-500";
    case "orange":
      return "text-orange-200 hover:text-orange-500 dark:text-orange-400/50 dark:hover:text-orange-500";
    case "purple":
      return "text-purple-200 hover:text-purple-500 dark:text-purple-400/50 dark:hover:text-purple-500";
    case "sky":
      return "text-sky-200 hover:text-sky-500 dark:text-sky-400/50 dark:hover:text-sky-500";
    case "slate":
      return "text-slate-200 hover:text-slate-500 dark:text-slate-400/50 dark:hover:text-slate-500";
    default:
      return "text-custom-200 hover:text-custom-500 dark:text-custom-400/50 dark:hover:text-custom-500";
  }
});

const prepareClass = computed(() => {
  if (props.outlined) {
    return "border " + outlinedClass.value;
  }
  if (props.bordered) {
    return "border " + borderedClass.value;
  }

  if (props.borderTop) {
    return "border border-0 border-t-2 " + topBorderClass.value;
  }

  if (props.solid) {
    return "border-transparent " + solidClass.value;
  }

  return "border-transparent " + defaultClass.value;
});

const onClick = (event: any) => {
  const element = event.target.closest("[alert]");
  if (element) {
    element.classList.add("hidden");
  }
};
</script>
<template>
  <div
    class="text-sm border px-4 py-3 rounded-md flex justify-between relative"
    alert
    :class="`${prepareClass} ${dismissible ? 'pr-12' : ''}`"
  >
    <span class="flex gap-2 text-wrap">
      <slot />
    </span>
    <button
      v-if="dismissible"
      :class="`absolute top-0 bottom-0 ltr:right-0 rtl:left-0 p-3 transition ${dismissibleClass}`"
      @click="onClick"
    >
      <X />
    </button>
  </div>
</template>
