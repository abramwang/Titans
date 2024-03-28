<script lang="ts" setup>
import NavBtn from "@/app/layout/navbar/Button.vue";
import { Settings } from "lucide-vue-next";
const emit = defineEmits(["toggleDrawer"]);
</script>
<template>
  <NavBtn @click="emit('toggleDrawer')">
    <Settings
      class="inline-block size-5 stroke-1 fill-slate-100 group-data-[topbar=dark]:fill-topbar-item-bg-hover-dark group-data-[topbar=brand]:fill-topbar-item-bg-hover-brand"
    />
  </NavBtn>
</template>
