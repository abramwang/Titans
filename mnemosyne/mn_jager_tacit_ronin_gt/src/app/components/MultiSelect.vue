<script lang="ts" setup>
import Multiselect from "@vueform/multiselect";
import { PropType, computed } from "vue";

const emit = defineEmits(["update:modelValue"]);
const props = defineProps({
  modelValue: {
    type: [String, Array] as PropType<string | string[]>,
    default: ""
  },
  options: {
    type: Array as PropType<{ value: string; label: string }[]>,
    default: () => []
  },
  mode: {
    type: String as PropType<"tags" | "multiple" | "single" | undefined>,
    default: "single"
  },
  multiple: {
    type: Boolean,
    default: false
  },
  searchable: {
    type: Boolean,
    default: false
  },
  groups: {
    type: Boolean,
    default: false
  }
});
const selectedValue = computed({
  get() {
    return props.modelValue;
  },
  set(newVal: string | string[]) {
    emit("update:modelValue", newVal);
  }
});
</script>
<template>
  <div class="multiselect-wrapper">
    <Multiselect
      v-model="selectedValue"
      :options="options"
      :mode="mode"
      :multiple="multiple"
      :close-on-select="false"
      :searchable="searchable"
      :groups="groups"
    />
  </div>
</template>
