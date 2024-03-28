<script lang="ts" setup>
defineProps({
  rows: {
    type: Number,
    default: 3
  },
  placeholder: {
    type: String,
    default: ""
  },
  label: {
    type: String,
    default: ""
  },
  required: {
    type: Boolean,
    default: false
  }
});
</script>
<template>
  <div>
    <div v-if="label">
      <label class="inline-block mb-2 text-base font-medium">
        {{ label }}
      </label>
      <span v-if="required" class="text-red-500">*</span>
    </div>
    <textarea
      class="form-input border-slate-200 dark:border-zink-500 focus:outline-none focus:border-custom-500 disabled:bg-slate-100 dark:disabled:bg-zink-600 disabled:border-slate-300 dark:disabled:border-zink-500 dark:disabled:text-zink-200 disabled:text-slate-500 dark:text-zink-100 dark:bg-zink-700 dark:focus:border-custom-800 placeholder:text-slate-400 dark:placeholder:text-zink-200"
      :rows="rows"
      :placeholder="placeholder"
    />
  </div>
</template>
