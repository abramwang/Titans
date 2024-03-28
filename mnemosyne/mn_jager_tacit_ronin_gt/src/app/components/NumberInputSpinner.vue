<script lang="ts" setup>
import { Minus, Plus } from "lucide-vue-next";

const emit = defineEmits(["onAdd", "onReduce"]);
const props = defineProps({
  modelValue: {
    type: Number,
    default: 0
  },
  min: {
    type: Number,
    default: 0
  },
  max: {
    type: Number,
    default: 100
  },
  variant: {
    type: String,
    default: "outlined"
  },
  color: {
    type: String,
    default: "light"
  },
  group: {
    type: Boolean,
    default: false
  },
  rounded: {
    type: Boolean,
    default: false
  }
});

const onReduce = () => {
  if (props.modelValue > props.min) {
    emit("onReduce");
  }
};

const onAdd = () => {
  if (props.modelValue < props.max) {
    emit("onAdd");
  }
};
</script>
<template>
  <div
    class="text-center input-step inline-flex border dark:border-zink-500 items-center"
    :class="`${!group ? 'px-2' : ''} ${
      rounded ? 'rounded-full' : 'rounded-md'
    }`"
  >
    <span
      :class="`${group ? 'ltr:border-r rtl:border-l border-slate-200 dark:border-zink-500' : ''}`"
    >
      <TButton
        icon
        :border="!group"
        class="p-0"
        :classes="`${
          !group ? '!h-7 !w-7' : 'ltr:!rounded-r-[0px] rtl:!rounded-l-[0px]'
        } `"
        :variant="variant"
        :rounded="rounded"
        :color="color"
        @click="onReduce"
      >
        <Minus class="inline-block size-4" />
      </TButton>
    </span>
    <div
      class="w-12 text-center items-center flex justify-center h-9 product-quantity dark:bg-zink-700 focus:shadow-none"
    >
      {{ modelValue }}
    </div>

    <span
      :class="`${group ? 'ltr:border-l rtl:border-r border-slate-200 dark:border-zink-500' : ''}`"
    >
      <TButton
        icon
        class="p-0"
        :variant="variant"
        :classes="`${
          !group ? '!h-7 !w-7' : 'ltr:!rounded-l-[0px] rtl:!rounded-r-[0px]'
        } `"
        :color="color"
        :rounded="rounded"
        :border="!group"
        @click="onAdd"
      >
        <Plus class="inline-block size-4" />
      </TButton>
    </span>
  </div>
</template>
