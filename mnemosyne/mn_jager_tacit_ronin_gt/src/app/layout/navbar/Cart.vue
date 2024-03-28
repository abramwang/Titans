<script lang="ts" setup>
import NavBtn from "@/app/layout/navbar/Button.vue";
import { ShoppingCart } from "lucide-vue-next";

const emit = defineEmits(["toggleDrawer"]);
</script>
<template>
  <NavBtn @click="emit('toggleDrawer')">
    <ShoppingCart
      class="inline-block size-5 stroke-1 fill-slate-100 group-data-[topbar=dark]:fill-topbar-item-bg-hover-dark group-data-[topbar=brand]:fill-topbar-item-bg-hover-brand"
    />
    <span
      class="absolute flex items-center justify-center w-[16px] h-[16px] text-xs text-white bg-red-400 border-white rounded-full -top-1 -right-1"
    >
      3
    </span>
  </NavBtn>
</template>
