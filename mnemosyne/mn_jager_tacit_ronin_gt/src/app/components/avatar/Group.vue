<script lang="ts" setup>
import { PropType } from "vue";

defineProps({
  images: {
    type: Array as PropType<string[]>,
    default: () => []
  },
  content: {
    type: [String],
    default: ""
  },
  contentAvatar: {
    type: Array as PropType<any>,
    default: () => []
  }
});
</script>
<template>
  <div class="flex flex-wrap items-center gap-4">
    <div class="flex -space-x-4">
      <img
        v-for="(item, index) in images"
        :key="'avatar-image-' + index"
        class="size-10 border-2 border-white rounded-full dark:border-zink-700"
        :src="item"
        :alt="'avatar-image-' + index"
      />
      <a
        v-if="content"
        class="flex items-center justify-center size-10 text-xs font-medium text-white border-2 border-white rounded-full bg-slate-700 hover:bg-slate-600 dark:border-zink-700"
        href="#"
      >
        {{ content }}
      </a>
      <div
        v-for="(item, index) in contentAvatar"
        :key="'content-avatar-' + index"
        class="flex items-center justify-center size-10 border-2 border-white rounded-full dark:border-zink-700"
        :class="item.class"
      >
        <span v-if="item.text">
          {{ item.text }}
        </span>
        <img
          v-else
          class="size-10 border-2 border-white rounded-full dark:border-zink-700"
          :src="item.src"
          :alt="'avatar-image-' + index"
        />
      </div>
    </div>
  </div>
</template>
