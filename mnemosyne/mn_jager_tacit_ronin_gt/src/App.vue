<script lang="ts" setup>
import { onMounted, computed } from "vue";
import { useLayoutStore } from "@/store/layout";
import { setAttribute } from "@/app/utils";

const layoutStore = computed(() => useLayoutStore());

onMounted(() => {
  const {
    mode,
    layoutType,
    layoutWidth,
    topBarColor,
    sideBarSize,
    sideBarColor,
    dir,
    skinLayout,
    navType
  } = layoutStore.value;

  setAttribute("data-mode", mode);
  setAttribute("data-topbar", topBarColor);
  setAttribute("data-sidebar", sideBarColor);
  setAttribute("data-layout", layoutType);
  setAttribute("data-skin", skinLayout);
  setAttribute("dir", dir);
  setAttribute("data-content", layoutWidth);
  setAttribute("data-sidebar-size", sideBarSize);
  setAttribute("data-navbar", navType);
});
</script>

<template>
  <component v-if="$route.meta.layout" :is="$route.meta.layout">
    <router-view />
  </component>
  <router-view v-else />
</template>
