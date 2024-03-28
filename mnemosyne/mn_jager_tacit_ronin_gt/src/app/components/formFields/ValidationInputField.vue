<script lang="ts" setup>
import { computed, PropType, watch } from "vue";
import { required, email, integer } from "@vuelidate/validators";
import { useVuelidate } from "@vuelidate/core";

const emit = defineEmits(["update:modelValue"]);
const props = defineProps({
  modelValue: {
    type: Object,
    default: {}
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
  rules: {
    type: Array as PropType<string[]>,
    default: () => []
  },
  submit: {
    type: Boolean,
    default: false
  }
});

const isValid = computed(() => {
  return v$.value.fieldValue.$errors.length === 0;
});

const rules = {
  fieldValue: {
    required: props.rules.includes("required") ? required : {},
    email: props.rules.includes("email") ? email : {},
    integer: props.rules.includes("integer") ? integer : {}
  }
};
const fieldValue = computed({
  get() {
    return props.modelValue.value;
  },
  set(newVal: string) {
    emit("update:modelValue", { value: newVal, isValid: isValid.value });
  }
});
const v$ = useVuelidate(rules, { fieldValue });
const onInput = (event: any) => {
  v$.value.$touch();
  fieldValue.value = event.target.value;
};
const isSubmitted = computed(() => props.submit);

watch(isSubmitted, () => {
  v$.value.$touch();
});
</script>
<template>
  <div class="flex flex-wrap gap-1" :class="!hideDetails ? 'mb-4' : ''">
    <TInputField
      class="w-full"
      v-model="fieldValue"
      :placeholder="placeholder"
      :label="label"
      required
      hide-details
      :type="type"
      @input="onInput"
    />
    <!-- :valid="isSubmitted && v$.fieldValue.$errors.length === 0"
      :invalid="isSubmitted && v$.fieldValue.$errors.length > 0" -->
    <div
      v-if="isSubmitted && v$.fieldValue.$invalid"
      class="text-red-600 text-sm"
    >
      <span v-for="error in v$.fieldValue.$errors">
        {{ error.$message }}
      </span>
    </div>
  </div>
</template>
