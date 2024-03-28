<script lang="ts" setup>
import {
  ChevronLeft,
  ChevronRight,
  ChevronsLeft,
  ChevronsRight
} from "lucide-vue-next";
import { computed } from "vue";

const emit = defineEmits(["update:modelValue"]);
const props = defineProps({
  modelValue: {
    type: Number,
    default: 1
  },
  totalPages: {
    type: Number,
    default: 5
  },
  pageClass: {
    type: String,
    default:
      "size-8 rounded text-slate-500 hover:text-custom-500 [&.active]:text-custom-500 [&.active]:hover:text-custom-500 [&.disabled]:text-slate-400 [&.disabled]:cursor-auto dark:text-zink-200 dark:bg-zink-700 dark:hover:text-custom-500 dark:[.active]:hover:text-custom-500"
  },
  actionClass: {
    type: String,
    default: ` size-8 rounded text-slate-500 hover:text-custom-500 [&.active]:text-custom-500 [&.active]:hover:text-custom-500 [&.disabled]:text-slate-400 [&.disabled]:cursor-auto dark:text-zink-200 dark:bg-zink-700 dark:hover:text-custom-500 dark:[.active]:hover:text-custom-500`
  },
  fastPagination: {
    type: Boolean,
    default: false
  }
});

const activePage = computed({
  get() {
    return props.modelValue;
  },
  set(newVal: number) {
    emit("update:modelValue", newVal);
  }
});

const onPrev = () => {
  if (activePage.value > 1) {
    activePage.value--;
  }
};

const onNext = () => {
  if (activePage.value < props.totalPages) {
    activePage.value++;
  }
};

const onFirstPage = () => {
  activePage.value = 1;
};

const onLastPage = () => {
  activePage.value = props.totalPages;
};
</script>
<template>
  <ul class="flex flex-wrap items-center gap-2">
    <li v-if="fastPagination">
      <a
        href="#!"
        class="transition-all duration-150 ease-linear inline-flex items-center justify-center bg-white dark:bg-zink-700"
        :class="` ${actionClass} ${activePage <= 1 ? 'disabled' : ''}`"
        @click="onFirstPage"
      >
        <ChevronsLeft class="size-4 rtl:rotate-180" />
      </a>
    </li>
    <li>
      <a
        href="#!"
        class="transition-all duration-150 ease-linear inline-flex items-center justify-center bg-white dark:bg-zink-700"
        :class="` ${actionClass} ${activePage <= 1 ? 'disabled' : ''}`"
        @click="onPrev"
      >
        <slot name="prev">
          <ChevronLeft class="size-4 rtl:rotate-180" />
        </slot>
      </a>
    </li>
    <li v-for="page in totalPages" :key="'page-' + page">
      <a
        href="#!"
        :class="`inline-flex items-center justify-center bg-white transition-all duration-150 ease-linear dark:bg-zink-700 ${
          activePage === page ? 'active' : ''
        } ${pageClass}`"
        @click="activePage = page"
      >
        {{ page }}
      </a>
    </li>

    <li>
      <a
        href="#!"
        class="inline-flex items-center justify-center bg-white transition-all duration-150 ease-linear dark:bg-zink-700"
        :class="`${actionClass} ${
          activePage > totalPages - 1 ? 'disabled' : ''
        }`"
        @click="onNext"
      >
        <slot name="next">
          <ChevronRight class="size-4 rtl:rotate-180" />
        </slot>
      </a>
    </li>
    <li v-if="fastPagination">
      <a
        href="#!"
        class="inline-flex items-center justify-center bg-white transition-all duration-150 ease-linear dark:bg-zink-700"
        :class="`${actionClass} ${
          activePage > totalPages - 1 ? 'disabled' : ''
        }`"
        @click="onLastPage"
      >
        <ChevronsRight class="size-4 rtl:rotate-180" />
      </a>
    </li>
  </ul>
</template>
