<script setup lang="ts">
import Layout from "@/app/layout/index.vue";
import { computed } from "vue";

const currentYear = computed(() => new Date().getFullYear());
</script>

<template>
  <Layout />
  <div class="relative min-h-screen group-data-[sidebar-size=sm]:min-h-sm">
    <div
      class="group-data-[sidebar-size=lg]:ltr:md:ml-vertical-menu group-data-[sidebar-size=lg]:rtl:md:mr-vertical-menu group-data-[sidebar-size=md]:ltr:ml-vertical-menu-md group-data-[sidebar-size=md]:rtl:mr-vertical-menu-md group-data-[sidebar-size=sm]:ltr:ml-vertical-menu-sm group-data-[sidebar-size=sm]:rtl:mr-vertical-menu-sm pt-[calc(theme('spacing.header')_*_1)] pb-[calc(theme('spacing.header')_*_0.8)] px-4 group-data-[navbar=bordered]:pt-[calc(theme('spacing.header')_*_1.3)] group-data-[navbar=hidden]:pt-0 group-data-[layout=horizontal]:mx-auto group-data-[layout=horizontal]:max-w-screen-2xl group-data-[layout=horizontal]:group-data-[sidebar-size=lg]:ltr:md:ml-auto group-data-[layout=horizontal]:group-data-[sidebar-size=lg]:rtl:md:mr-auto group-data-[layout=horizontal]:md:pt-[calc(theme('spacing.header')_*_1.8)] group-data-[layout=horizontal]:px-3 group-data-[layout=horizontal]:group-data-[navbar=hidden]:pt-[calc(theme('spacing.header')_*_0.9)]"
    >
      <div
        class="container-fluid group-data-[content=boxed]:max-w-boxed mx-auto relative"
      >
        <router-view />
      </div>
    </div>
    <footer
      class="ltr:md:left-vertical-menu rtl:md:right-vertical-menu group-data-[sidebar-size=md]:ltr:md:left-vertical-menu-md group-data-[sidebar-size=md]:rtl:md:right-vertical-menu-md group-data-[sidebar-size=sm]:ltr:md:left-vertical-menu-sm group-data-[sidebar-size=sm]:rtl:md:right-vertical-menu-sm absolute right-0 bottom-0 px-4 h-14 group-data-[layout=horizontal]:ltr:left-0 group-data-[layout=horizontal]:rtl:right-0 left-0 border-t py-3 flex items-center dark:border-zink-600"
    >
      <div
        class="group-data-[layout=horizontal]:mx-auto group-data-[layout=horizontal]:max-w-screen-2xl w-full"
      >
        <div
          class="grid items-center grid-cols-1 text-center lg:grid-cols-2 text-slate-400 dark:text-zink-200 ltr:lg:text-left rtl:lg:text-right"
        >
          <div>{{ currentYear }} © Tailwick.</div>
          <div class="hidden lg:block">
            <div class="ltr:text-right rtl:text-left">
              Design &amp; Develop by Themesdesign
            </div>
          </div>
        </div>
      </div>
    </footer>
  </div>
</template>
