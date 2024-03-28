<script setup lang="ts">
import { type PropType, ref, computed } from "vue";
import { ListMenuItemType } from "@/app/components/list/types";

const emit = defineEmits(["onSelect"]);
const props = defineProps({
  modelValue: {
    type: [String, Object],
    default: ""
  },
  items: {
    type: Array as PropType<ListMenuItemType[] | string[]>,
    default: () => []
  },
  absolute: {
    type: Boolean,
    default: true
  },
  shadow: {
    type: Boolean,
    default: true
  },
  color: {
    type: String,
    default: "slate"
  },
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

const onSelect = (item: any, close: any) => {
  emit("onSelect", item);
  close();
};
const list = ref<any>(null);

const getListItemClass = computed(() => {
  switch (props.color) {
    case "custom":
      return "text-custom-400 hover:text-custom-600 focus:text-custom-600 dark:text-custom-500 dark:hover:text-custom-500 dark:focus:text-custom-500";
    case "yellow":
      return "text-yellow-400 hover:text-yellow-600 focus:text-yellow-600 dark:text-yellow-500 dark:hover:text-yellow-500 dark:focus:text-yellow-500";
    case "green":
      return "text-green-400 hover:text-green-600 focus:text-green-600 dark:text-green-500 dark:hover:text-green-500 dark:focus:text-green-500";
    case "red":
      return "text-red-400 hover:text-red-600 focus:text-red-600 dark:text-red-500 dark:hover:text-red-500 dark:focus:text-red-500";
    case "orange":
      return "text-orange-400 hover:text-orange-600 focus:text-orange-600 dark:text-orange-500 dark:hover:text-orange-500 dark:focus:text-orange-500";
    case "purple":
      return "text-purple-400 hover:text-purple-600 focus:text-purple-600 dark:text-purple-500 dark:hover:text-purple-500 dark:focus:text-purple-500";
    case "sky":
      return "text-sky-400 hover:text-sky-600 focus:text-sky-600 dark:text-sky-500 dark:hover:text-sky-500 dark:focus:text-sky-500";
    case "slate":
      return "text-slate-400 hover:text-slate-600 focus:text-slate-600 dark:text-slate-500 dark:hover:text-slate-500 dark:focus:text-slate-500 text-slate-600  hover:bg-slate-100 hover:text-slate-500 focus:bg-slate-100 focus:text-slate-500 ";

    default:
      return "text-slate-600 dark:text-zink-200 group-hover/items:text-custom-500";
  }
});
</script>
<template>
  <Popper :placement="placement">
    <slot name="title" />
    <template #content="{ close }">
      <div class="bg-white dark:bg-zink-600 dropdown-menu shadow-md rounded-md">
        <ul
          ref="list"
          class="transition-[height] duration-200 bg-white z-50 py-2 dropdown-menu min-w-[10rem] ltr:text-left rtl:text-right rounded-md dropdown-menu flex flex-col dark:bg-zink-600"
          :class="{
            shadow: shadow ? 'shadow-md' : '',
            'ltr:right-0 rtl:-right-24': placement === 'bottom-end'
          }"
        >
          <li
            v-for="(item, index) in items"
            :key="index"
            class="cursor-pointer"
            @click="onSelect(item, close)"
          >
            <div
              class="block px-4 py-1.5 text-base transition-all duration-200 ease-linear dropdown-item dark:text-zink-100 dark:hover:bg-zink-500 dark:hover:text-zink-200 dark:focus:bg-zink-500 dark:focus:text-zink-200"
              :class="getListItemClass"
            >
              <slot :data="item" :index="index">
                <div class="text-md">
                  {{ typeof item === "object" ? item.title : item }}
                </div>
              </slot>
            </div>
          </li>
        </ul>
      </div>
    </template>
  </Popper>
</template>
