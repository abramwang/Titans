<script lang="ts" setup>
import { onMounted } from "vue";
import LanguageDropdown from "@/app/layout/navbar/Language.vue";

onMounted(() => {
  document.body.setAttribute(
    "class",
    "flex items-center justify-center min-h-screen py-16 bg-cover bg-auth-pattern dark:bg-auth-pattern-dark dark:text-zink-100 font-public px-4"
  );
});
</script>
<template>
  <div
    class="mb-0 border-none shadow-none whitespace-nowrap card xl:w-2/3 bg-white/70 dark:bg-zink-500/70 mx-auto"
  >
    <div class="grid grid-cols-1 gap-0 lg:grid-cols-12">
      <div class="lg:col-span-5 my-auto">
        <div class="!px-12 !py-12 card-body">
          <router-view />
        </div>
      </div>
      <div
        class="mx-2 mt-2 mb-2 border-none shadow-none lg:col-span-7 card bg-white/60 dark:bg-zink-500/60 relative"
      >
        <div class="!px-10 !pt-10 h-full !pb-0 card-body flex flex-col">
          <div class="flex items-center justify-between gap-3">
            <div class="grow">
              <a href="index">
                <img
                  src="@/assets/images/logo-light.png"
                  alt=""
                  class="hidden h-6 dark:block"
                />
                <img
                  src="@/assets/images/logo-dark.png"
                  alt=""
                  class="block h-6 dark:hidden"
                />
              </a>
            </div>
            <div>
              <LanguageDropdown show-name />
            </div>
          </div>
          <div class="mt-auto">
            <img
              src="@/assets/images/auth/img-01.png"
              alt="auth-img"
              class="md:max-w-[32rem] mx-auto"
            />
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style lang="scss">
#app {
  width: 100%;
}
</style>
