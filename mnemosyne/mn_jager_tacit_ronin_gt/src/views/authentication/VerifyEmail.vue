<script lang="ts" setup>
import { LAYOUT_TYPES } from "@/layouts/types.ts";

defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});
</script>
<template>
  <div class="mt-8 text-center">
    <h4 class="mb-1 text-custom-500 dark:text-custom-500">Verify Email</h4>
    <p class="mb-4 text-slate-500 dark:text-zink-200">
      Did you not receive an email? Please
      <a href="#!" class="text-custom-500">try again</a>
    </p>
    <TButton class="px-2 py-1.5 text-xs mx-auto"> Skip Now </TButton>
  </div>

  <div class="text-center pt-10">
    <img
      src="@/assets/images/auth-email.png"
      alt=""
      class="block mx-auto w-2/3"
    />
  </div>
</template>
