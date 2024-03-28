<script lang="ts" setup>
import { onMounted } from "vue";
import BasicLayoutBg from "@/assets/images/svg/BasicLayoutBg.vue";
onMounted(() => {
  document.body.setAttribute(
    "class",
    "flex items-center justify-center min-h-screen py-16 lg:py-10 bg-slate-50 dark:bg-zink-800 dark:text-zink-100 font-public"
  );
});
</script>
<template>
  <div class="relative">
    <div
      class="absolute hidden opacity-50 -left-16 rtl:-right-16 -top-10 md:block"
    >
      <BasicLayoutBg />
    </div>

    <div
      class="absolute hidden -rotate-180 opacity-50 -right-16 rtl:-left-16 -bottom-10 md:block"
    >
      <BasicLayoutBg />
    </div>

    <div
      class="mb-0 w-screen lg:mx-auto lg:w-[500px] card shadow-lg border-none shadow-slate-100 relative"
    >
      <div class="!px-10 !py-12 card-body">
        <a href="#!">
          <img
            src="@/assets/images/logo-light.png"
            alt=""
            class="hidden h-6 mx-auto dark:block"
          />
          <img
            src="@/assets/images/logo-dark.png"
            alt=""
            class="block h-6 mx-auto dark:hidden"
          />
        </a>

        <div class="mt-8">
          <router-view />
        </div>
      </div>
    </div>
  </div>
</template>
