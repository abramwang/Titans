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
  <div class="text-center">
    <h4 class="mb-2 text-custom-500 dark:text-custom-500">Forgot Password?</h4>
    <p class="mb-8 text-slate-500 dark:text-zink-200">
      Reset your Tailwick password
    </p>
  </div>

  <div
    class="px-4 py-3 mb-6 text-sm text-yellow-500 border border-transparent rounded-md bg-yellow-50 dark:bg-yellow-400/20"
  >
    Provide your email address, and instructions will be sent to you
  </div>

  <form autocomplete="off" action="/">
    <TInputField label="Email" placeholder="Enter Email" required />

    <div class="mt-8">
      <TButton type="submit" class="w-full"> Send Reset Link </TButton>
    </div>
    <div class="mt-4 text-center">
      <p class="mb-0">
        Wait, I remember my password...
        <router-link
          :to="`/login/${layout}`"
          class="underline fw-medium text-custom-500"
        >
          Click here
        </router-link>
      </p>
    </div>
  </form>
</template>
