<script lang="ts" setup>
import { computed, type PropType, watch } from "vue";

const emit = defineEmits(["update:modelValue"]);
const props = defineProps({
  modelValue: {
    type: Boolean,
    default: false
  },
  location: {
    type: String as PropType<"modal-center" | "modal-top" | "modal-bottom">,
    default: "modal-center"
  },
  size: {
    type: String as PropType<
      "x-small" | "small" | "default" | "large" | "x-large" | "full"
    >,
    default: "default"
  }
});

const isVisible = computed({
  get() {
    return props.modelValue;
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

const getClasses = computed(() => {
  let classes = "";

  if (!isVisible.value) {
    classes = `${classes} show`;
  }

  if (props.size !== "full") {
    classes = `${classes} fixed flex flex-col transition-all duration-300 ease-in-out left-2/4 z-drawer -translate-x-2/4`;
  } else {
    classes = `${classes} fixed inset-0 flex flex-col transition-all duration-300 ease-in-out z-drawer ${
      isVisible.value ? "" : "hidden"
    }`;
  }

  if (props.location === "modal-center" && props.size !== "full") {
    classes = `${classes}  -translate-y-2/4 `;
  }
  return classes;
});
</script>
<template>
  <div
    :modal-center="location === 'modal-center'"
    :modal-top="location === 'modal-top'"
    :modal-bottom="location === 'modal-bottom'"
    :class="getClasses"
  >
    <div
      v-if="isVisible"
      class="w-screen bg-white shadow rounded-md dark:bg-zink-600 flex flex-col h-full"
      :class="{
        'md:w-[30rem]': size === 'default',
        'md:w-[25rem] ': size === 'x-small',
        'md:w-[28rem] ': size === 'small',
        'md:w-[40rem] ': size === 'large',
        'lg:w-[55rem] ': size === 'x-large',
        'h-full w-full': size === 'full'
      }"
    >
      <slot name="content" />
    </div>
  </div>
  <Teleport to="body">
    <div
      v-if="isVisible"
      class="fixed inset-0 bg-slate-900/40 dark:bg-zink-800/70 z-[1049] backdrop-overlay"
      @click="isVisible = false"
    ></div>
  </Teleport>
</template>
