<script lang="ts" setup>
import TwoStepForm from "@/components/authentication/TwoStepForm.vue";
import { LAYOUT_TYPES } from "@/layouts/types.ts";

defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});
</script>
<template>
  <TwoStepForm :layout="layout" />
</template>
