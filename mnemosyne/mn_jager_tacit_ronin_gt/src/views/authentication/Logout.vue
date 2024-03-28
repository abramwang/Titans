<script lang="ts" setup>
import { LogOut } from "lucide-vue-next";
import { useRouter } from "vue-router";
import { LAYOUT_TYPES } from "@/layouts/types.ts";

const router = useRouter();
const props = defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});

const onClick = () => {
  router.push({ path: `/login/${props.layout}` });
};
</script>
<template>
  <div class="mt-8 text-center">
    <div class="mb-4 text-center">
      <LogOut class="size-6 mx-auto text-purple-500 fill-purple-100" />
    </div>
    <h4 class="mb-2 text-custom-500 dark:text-custom-500">
      You are Logged Out
    </h4>
    <p class="mb-8 text-slate-500 dark:text-zink-200 text-wrap">
      Thank you for using tailwick admin template
    </p>
  </div>

  <TButton @click="onClick" class="w-full"> Sign In </TButton>
</template>
