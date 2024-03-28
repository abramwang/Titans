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
    <h4 class="mb-2 text-custom-500 dark:text-custom-500">
      Set a New Password
    </h4>
    <p class="mb-8 text-slate-500 dark:text-zink-200 whitespace-normal">
      Your new password should be distinct from any of your prior passwords
    </p>
  </div>

  <form autocomplete="off" action="/">
    <TInputField
      label="Password"
      placeholder="Password"
      required
      type="password"
    />
    <TInputField
      label="Confirm Password"
      placeholder="Confirm password"
      required
      type="password"
    />
    <TCheckbox id="checkboxDefault1">
      <span
        class="inline-block text-base font-medium align-middle cursor-pointer"
      >
        Remember me
      </span>
    </TCheckbox>
    <div class="mt-8">
      <TButton class="w-full" type="submit"> Reset Password </TButton>
    </div>
    <div class="mt-4 text-center">
      <p class="mb-0">
        Hold on, I've got my password...
        <router-link
          :to="`/login/${layout}`"
          class="font-semibold underline transition-all duration-150 ease-linear text-slate-500 dark:text-zink-200 hover:text-custom-500 dark:hover:text-custom-500"
        >
          Click here
        </router-link>
      </p>
    </div>
  </form>
</template>
