<script lang="ts" setup>
import { computed } from "vue";
const emit = defineEmits(["update:modelValue"]);
const props = defineProps({
  modelValue: {
    type: [String, Number],
    default: ""
  },
  required: {
    type: Boolean,
    default: false
  },
  placeholder: {
    type: String,
    default: ""
  },
  type: {
    type: String,
    default: "text"
  },
  label: {
    type: String,
    default: ""
  },
  hideDetails: {
    type: Boolean,
    default: false
  },
  readonly: {
    type: Boolean,
    default: false
  },
  border: {
    type: Boolean,
    default: true
  },
  disabled: {
    type: Boolean,
    default: false
  },
  inputClass: {
    type: String,
    default: ""
  },
  tipText: {
    type: String,
    default: ""
  },
  valid: {
    type: Boolean,
    default: false
  },
  invalid: {
    type: Boolean,
    default: false
  }
});
const value = computed({
  get() {
    return props.modelValue;
  },
  set(newVal: string | number) {
    emit("update:modelValue", newVal);
  }
});
const getBorderClass = computed(() => {
  const { border, valid, invalid } = props;
  if (border) {
    if (valid) {
      return "border-green-500 dark:border-green-800";
    } else if (invalid) {
      return "border border-red-500 dark:border-red-800";
    }
    return "border-slate-200 dark:border-zink-500 disabled:border-slate-300 focus:border-custom-500 dark:disabled:border-zink-500 dark:focus:border-custom-800";
  }
  return "border-transparent";
});
</script>
<template>
  <div :class="!hideDetails ? 'mb-4' : ''">
    <div v-if="label">
      <label class="inline-block mb-2 text-base font-medium">
        {{ label }}
      </label>
      <span v-if="required" class="text-red-500">*</span>
    </div>
    <div class="relative">
      <span class="absolute inline-block">
        <slot name="prefix-outer" />
      </span>
      <span
        class="px-3 absolute flex items-center h-full ltr:left-0 rtl:right-0 top-0 text-slate-500 dark:text-zink-200"
      >
        <slot name="prefix" />
      </span>
      <input
        v-model="value"
        :type="type"
        class="form-input focus:outline-none disabled:bg-slate-100 dark:disabled:bg-zink-600 dark:disabled:text-zink-200 disabled:text-slate-500 dark:text-zink-100 dark:bg-zink-700 placeholder:text-slate-400 dark:placeholder:text-zink-200"
        :class="`${inputClass} ${getBorderClass}`"
        :placeholder="placeholder"
        :required="required"
        :readonly="readonly"
        :disabled="disabled"
      />
      <span
        class="absolute flex items-center h-full top-0 rtl:left-3 ltr:right-3 px-3 text-slate-500 dark:text-zink-200"
      >
        <slot name="suffix" />
      </span>
      <div
        v-if="tipText"
        class="mt-1 text-sm text-slate-400 dark:text-zink-200"
      >
        {{ tipText }}
      </div>
      <span class="absolute inline-block ltr:right-0 rtl:left-0 top-0">
        <slot name="suffix-outer" />
      </span>
    </div>
  </div>
</template>
