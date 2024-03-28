<script lang="ts" setup>
import flatPickr from "vue-flatpickr-component";
import { computed } from "vue";

const emit = defineEmits(["update:modelValue"]);

const props = defineProps({
  modelValue: {
    type: [String, Date],
    default: ""
  },
  config: {
    type: Object,
    default: {
      altFormat: "M j, Y",
      dateFormat: "d/m/Y"
    }
  },
  required: {
    type: Boolean,
    default: false
  },
  label: {
    type: String,
    default: ""
  },
  hideDetails: {
    type: Boolean,
    default: false
  },
  valid: {
    type: Boolean,
    default: false
  },
  invalid: {
    type: Boolean,
    default: false
  },
  inputClass: {
    type: String,
    default: ""
  }
});

const date = computed({
  get() {
    return props.modelValue;
  },
  set(newVal: string | Date) {
    emit("update:modelValue", newVal);
  }
});

const getBorderClass = computed(() => {
  const { valid, invalid } = props;
  if (valid) {
    return "border-green-500 dark:border-green-800";
  } else if (invalid) {
    return "border border-red-500 dark:border-red-800";
  }
  return "border-slate-200 dark:border-zink-500 focus:outline-none focus:border-custom-500 disabled:bg-slate-100 dark:disabled:bg-zink-600 disabled:border-slate-300 dark:disabled:border-zink-500 dark:disabled:text-zink-200 disabled:text-slate-500 dark:text-zink-100 dark:bg-zink-700 dark:focus:border-custom-800 placeholder:text-slate-400 dark:placeholder:text-zink-200";
});
</script>
<template>
  <div class="relative" :class="`${!hideDetails ? 'mb-4' : ''}`">
    <label v-if="label" class="inline-block mb-2 text-base font-medium">
      {{ label }}
    </label>
    <span v-if="required" class="text-red-500">*</span>
    <flat-pickr
      v-model="date"
      :config="config"
      class="form-input"
      :class="`${getBorderClass} ${inputClass}`"
      placeholder="Select date"
      name="date"
    />
    <slot />
  </div>
</template>
