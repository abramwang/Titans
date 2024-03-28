<script setup lang="ts">
import { ref } from "vue";

const props = defineProps({
  class: {
    type: String,
    default: ""
  },
  visible: {
    type: Boolean,
    default: false
  },
  hideCard: {
    type: Boolean,
    default: false
  }
});

const isCollapsed = ref(!props.visible);

const onToggleCollapse = () => {
  isCollapsed.value = !isCollapsed.value;
};
</script>
<template>
  <div class="mb-4">
    <slot
      name="actionBtn"
      :onClick="onToggleCollapse"
      :isCollapsed="isCollapsed"
    />
    <div
      v-show="!isCollapsed"
      class="transition-transform transform origin-top ease-out duration-300 mt-2"
    >
      <TCard v-if="!hideCard">
        <slot name="content" />
      </TCard>
      <slot v-else name="content" />
    </div>
  </div>
</template>
