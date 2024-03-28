<script lang="ts" setup>
import { computed, watch } from "vue";
import { required } from "@vuelidate/validators";
import { useVuelidate } from "@vuelidate/core";

const emit = defineEmits(["update:modelValue"]);
const props = defineProps({
  modelValue: {
    type: Object,
    default: {}
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
  submit: {
    type: Boolean,
    default: false
  }
});

const rules = {
  fieldValue: {
    required: props.required ? required : {}
  }
};

const isSubmitted = computed(() => props.submit);

const fieldValue = computed({
  get() {
    return props.modelValue.value;
  },
  set(newVal: string) {
    emit("update:modelValue", { value: newVal, isValid: isValid.value });
  }
});

watch(isSubmitted, () => {
  v$.value.$touch();
});

const v$ = useVuelidate(rules, { fieldValue });

const isValid = computed(() => v$.value.fieldValue.$errors.length === 0);
</script>
<template>
  <div>
    <TFlatPicker
      v-model="fieldValue"
      :label="label"
      :required="required"
      :valid="isSubmitted && v$.fieldValue.$errors.length === 0"
      :invalid="isSubmitted && v$.fieldValue.$errors.length > 0"
      hide-details
      class="w-100"
    />
    <div v-if="isSubmitted && v$.fieldValue.$invalid" class="text-red-600 my-1">
      <span v-for="error in v$.fieldValue.$errors">
        {{ error.$message }}
      </span>
    </div>
  </div>
</template>
