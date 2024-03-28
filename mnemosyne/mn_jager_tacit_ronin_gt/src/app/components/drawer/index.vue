<script lang="ts" setup>
import { computed, type PropType, watch } from "vue";
import { X } from "lucide-vue-next";

const emit = defineEmits(["update:modelValue"]);
const prop = defineProps({
  modelValue: {
    type: Boolean,
    default: false
  },
  location: {
    type: String as PropType<
      "drawer-end" | "drawer-start" | "drawer-bottom" | "drawer-top"
    >,
    default: "drawer-end"
  },
  width: {
    type: String,
    default: "md:w-80"
  }
});

const isVisible = computed({
  get() {
    return prop.modelValue;
  },
  set(newVal: boolean) {
    emit("update:modelValue", newVal);
  }
});

watch(isVisible, (newVal: boolean) => {
  if (newVal) {
    document.body.classList.add("overflow-hidden");
  } else {
    document.body.classList.remove("overflow-hidden");
  }
});

const getDrawerClasses = computed(() => {
  let classes =
    "fixed flex flex-col w-full transition-transform duration-300 ease-in-out transform bg-white shadow dark:bg-zink-600 z-drawer";
  if (!isVisible.value) {
    classes = `${classes} show`;
  }

  if (prop.location === "drawer-end") {
    classes = `${classes} inset-y-0 ltr:right-0 rtl:left-0 ${prop.width}`;
  } else if (prop.location === "drawer-start") {
    classes = `${classes} inset-y-0 ltr:left-0 rtl:right-0 ${prop.width}`;
  } else if (prop.location === "drawer-bottom") {
    classes = `${classes} bottom-0 left-0 right-0 md:h-80`;
  } else if (prop.location === "drawer-top") {
    classes = `${classes} top-0 left-0 right-0 md:h-80`;
  }

  return classes;
});
</script>
<template>
  <div
    :class="getDrawerClasses"
    drawer
    :drawer-end="location === 'drawer-end'"
    :drawer-start="location === 'drawer-start'"
    :drawer-bottom="location === 'drawer-bottom'"
    :drawer-top="location === 'drawer-top'"
  >
    <div
      class="flex items-center justify-between p-4 border-b border-slate-200 dark:border-zink-500"
    >
      <div><slot name="title" /></div>
      <div class="shrink-0">
        <button
          class="transition-all duration-150 ease-linear text-slate-500 hover:text-slate-800"
          @click="isVisible = false"
        >
          <X class="size-4" />
        </button>
      </div>
    </div>
    <slot name="content" />
  </div>
  <Teleport to="body">
    <div
      v-if="isVisible"
      class="fixed inset-0 bg-slate-900/40 dark:bg-zink-800/70 z-[1049] backdrop-overlay"
      @click="isVisible = false"
    ></div>
  </Teleport>
</template>
