<script lang="ts" setup>
import RegisterForm from "@/components/authentication/RegisterForm.vue";
import { LAYOUT_TYPES } from "@/layouts/types.ts";

defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});
</script>
<template>
  <RegisterForm :layout="layout" />
</template>
