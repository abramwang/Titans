<script lang="ts" setup>
import { type PropType } from "vue";
defineProps({
  placement: {
    type: String as PropType<
      | "bottom-end"
      | "auto"
      | "auto-start"
      | "auto-end"
      | "top"
      | "top-start"
      | "top-end"
      | "bottom"
      | "bottom-start"
      | "right"
      | "right-start"
      | "right-end"
      | "left"
      | "left-start"
      | "left-end"
    >,
    default: "bottom-end"
  }
});
</script>
<template>
  <Popper :placement="placement">
    <slot />
    <template #content="{ close }">
      <div class="bg-white dark:bg-zink-600 dropdown-menu shadow-md rounded-md">
        <slot name="content" :close="close" />
      </div>
    </template>
  </Popper>
</template>
