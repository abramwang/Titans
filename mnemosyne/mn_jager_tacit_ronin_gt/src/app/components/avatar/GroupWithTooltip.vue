<script lang="ts" setup>
import { PropType, onMounted } from "vue";
// import initVendorScript from "@/assets/js/tailwick.bundle.js";

defineProps({
  images: {
    type: Array as PropType<{ src?: string; content: string }[]>,
    default: () => []
  },
  content: {
    type: String,
    default: ""
  },
  contentAvatar: {
    type: Array as PropType<
      { content: string; class: string; text: string; src?: string }[]
    >,
    default: () => []
  }
});

onMounted(() => {
  // initVendorScript();
});
</script>
<template>
  <div class="flex flex-wrap items-center gap-4">
    <div class="flex -space-x-4">
      <a
        v-for="(item, index) in images"
        :key="'avatar-group-image-' + index"
        href="#!"
        v-tippy="item.content"
      >
        <img
          class="size-10 border-2 border-white rounded-full dark:border-zink-700"
          :src="item.src"
        />
      </a>
      <a
        v-if="content"
        class="flex items-center justify-center size-10 text-xs font-medium text-white border-2 border-white rounded-full bg-slate-700 hover:bg-slate-600 dark:border-zink-700"
        href="#"
        v-tippy="'More Users'"
      >
        {{ content }}
      </a>

      <a
        v-for="(item, index) in contentAvatar"
        :key="'avatar-group-image-' + index"
        href="#!"
        v-tippy="item.content"
      >
        <div
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
      </a>
    </div>
  </div>
</template>
