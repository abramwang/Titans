<script lang="ts" setup>
import { computed } from "vue";
import NavBtn from "@/app/layout/navbar/Button.vue";
import { Sun, Moon } from "lucide-vue-next";
import { useLayoutStore } from "@/store/layout";
import { SITE_MODE } from "@/app/const";

const { DARK, LIGHT } = SITE_MODE;
const layout = useLayoutStore();

const siteMode = computed(() => layout.mode);

const isDarkMode = computed(() => {
  return siteMode.value === DARK;
});

const isLightMode = computed(() => {
  return siteMode.value === LIGHT;
});
const onClick = () => {
  const siteMode = isDarkMode.value ? LIGHT : DARK;
  layout.changeSiteMode(siteMode);
};
</script>
<template>
  <NavBtn @click="onClick">
    <Sun
      class="inline-block size-5 stroke-1 fill-slate-100 group-data-[topbar=dark]:fill-topbar-item-bg-hover-dark group-data-[topbar=brand]:fill-topbar-item-bg-hover-brand"
      :class="{
        hidden: isDarkMode
      }"
    />
    <Moon
      class="inline-block size-5 stroke-1 fill-slate-100 group-data-[topbar=dark]:fill-topbar-item-bg-hover-dark group-data-[topbar=brand]:fill-topbar-item-bg-hover-brand"
      :class="{
        hidden: isLightMode
      }"
    />
  </NavBtn>
</template>
