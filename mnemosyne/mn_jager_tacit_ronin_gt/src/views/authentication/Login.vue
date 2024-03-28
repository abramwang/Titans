<script lang="ts" setup>
import LoginForm from "@/components/authentication/LoginForm.vue";
import { LAYOUT_TYPES } from "@/layouts/types.ts";

defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});
</script>
<template>
  <LoginForm :layout="layout" />
</template>
