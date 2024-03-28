<script lang="ts" setup>
import ResetPasswordForm from "@/components/authentication/ResetPasswordForm.vue";
import { LAYOUT_TYPES } from "@/layouts/types.ts";

defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});
</script>
<template>
  <ResetPasswordForm :layout="layout" />
</template>
