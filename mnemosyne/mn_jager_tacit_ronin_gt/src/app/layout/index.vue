<script lang="ts" setup>
import { onMounted, ref, onBeforeUnmount } from "vue";
import { setAttribute } from "@/app/utils";
import NavBar from "@/app/layout/navbar/index.vue";
import MenuComponent from "@/app/layout/menu/index.vue";
import { Settings } from "lucide-vue-next";
import CustomizerDrawer from "@/app/layout/navbar/customizer/Drawer.vue";

const customizerDrawer = ref(false);
onMounted(() => {
  setAttribute("class", "light scroll-smooth group");
  document.body.setAttribute(
    "class",
    "text-base bg-body-bg text-body font-public dark:text-zink-100 dark:bg-zink-800 group-data-[skin=bordered]:bg-body-bordered group-data-[skin=bordered]:dark:bg-zink-700"
  );

  document.addEventListener("scroll", function () {
    windowScroll();
  });
});

const windowScroll = () => {
  var navbar = document.getElementById("page-topbar");
  if (navbar) {
    if (
      document.body.scrollTop >= 50 ||
      document.documentElement.scrollTop >= 50
    ) {
      navbar.classList.add("is-sticky");
    } else {
      navbar.classList.remove("is-sticky");
    }
  }
};
onBeforeUnmount(() => {
  document.removeEventListener("scroll", function () {
    windowScroll();
  });
});
</script>
<template>
  <NavBar />
  <MenuComponent />
  <div
    class="fixed items-center bottom-6 ltr:right-12 rtl:left-12 h-header z-[99] hidden group-data-[navbar=hidden]:flex"
  >
    <TButton
      icon
      color="sky"
      class="!w-12 !h-12 p-0"
      @click="customizerDrawer = true"
    >
      <Settings class="inline-block size-5" />
    </TButton>
  </div>
  <TDrawer v-model="customizerDrawer" width="md:w-96">
    <template #title>
      <div class="flex justify-between border-slate-200 dark:border-zink-500">
        <div class="grow">
          <h5 class="mb-1 text-16">Tailwick Theme Customizer</h5>
          <p class="font-normal text-slate-500 dark:text-zink-200">
            Choose your themes & layouts etc.
          </p>
        </div>
      </div>
    </template>
    <template #content>
      <CustomizerDrawer v-if="customizerDrawer" />
    </template>
  </TDrawer>
</template>
