<script lang="ts" setup>
import { onMounted } from "vue";
import { Home } from "lucide-vue-next";

onMounted(() => {
  document.body.setAttribute(
    "class",
    "flex items-center justify-center min-h-screen py-16 bg-cover bg-auth-pattern dark:bg-auth-pattern-dark font-public"
  );
});
</script>
<template>
  <div class="mt-10">
    <img
      src="@/assets/images/auth/maintenance.png"
      alt=""
      class="mx-auto h-72"
    />
  </div>
  <div class="mt-8 text-center">
    <h4 class="mb-2 text-purple-500">Currently in Maintenance Mode !!!</h4>
    <p class="mb-6 text-slate-500 dark:text-zink-200">
      Website is Under Construction. Please check back later!
    </p>
    <router-link
      to="/"
      class="text-white btn bg-custom-500 border-custom-500 hover:text-white hover:bg-custom-600 hover:border-custom-600 focus:text-white focus:bg-custom-600 focus:border-custom-600 focus:ring focus:ring-custom-100 active:text-white active:bg-custom-600 active:border-custom-600 active:ring active:ring-custom-100 dark:ring-custom-400/20"
    >
      <Home class="inline-block size-3 mr-2 rtl:ml-1" />
      <span class="align-middle">Back to Home</span>
    </router-link>
  </div>
</template>
