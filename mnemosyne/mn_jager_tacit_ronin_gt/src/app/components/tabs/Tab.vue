<script lang="ts" setup>
import { inject } from "vue";
const injectedTabData: any = inject("tabs");
const { onTabChange, activeTab } = injectedTabData;

defineProps({
  value: {
    type: String,
    required: true
  },
  tabClass: {
    type: String,
    default: ""
  },
  grow: {
    type: Boolean,
    default: false
  }
});
const onClick = (data: any) => {
  onTabChange(data);
};
</script>
<template>
  <div
    class="group cursor-pointer group/item tabs"
    :class="{
      active: activeTab === value,
      grow: grow
    }"
  >
    <div :class="`${tabClass} ${activeTab === value ? 'active' : ''}`" @click="onClick(value)">
      <slot />
    </div>
  </div>
</template>
