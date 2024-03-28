<script lang="ts" setup>
import CreatePasswordForm from "@/components/authentication/CreatePasswordForm.vue";
import { LAYOUT_TYPES } from "@/layouts/types.ts";

defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});
</script>
<template>
  <CreatePasswordForm :layout="layout" />
</template>
