<script lang="ts" setup>
import { computed, ref, watch } from "vue";
import { Check } from "lucide-vue-next";
import VerticalLabel from "@/app/layout/navbar/customizer/VerticalLabel.vue";
import HorizontalLabel from "@/app/layout/navbar/customizer/HorizontalLabel.vue";
import SkinDefault from "@/app/layout/navbar/customizer/SkinDefault.vue";
import SkinBordered from "@/app/layout/navbar/customizer/SkinBordered.vue";
import Button from "@/app/layout/navbar/customizer/Button.vue";
import { useLayoutStore } from "@/store/layout";
import {
  LAYOUTS,
  LAYOUT_WIDTH,
  SITE_MODE,
  TOP_BAR,
  SIDEBAR_SIZE,
  SIDEBAR_COLOR,
  SKIN_LAYOUT,
  DIR,
  NAV_BAR_TYPE
} from "@/app/const";

const layoutStore = computed(() => {
  return useLayoutStore();
});

const layoutType = computed({
  get() {
    return layoutStore.value.layoutType;
  },
  set(newVal: string) {
    layoutStore.value.changeLayoutType(newVal);
  }
});
const isVerticalLayout = computed(() => layoutType.value === LAYOUTS.VERTICAL);

const skinLayoutType = computed({
  get() {
    return layoutStore.value.skinLayout;
  },
  set(newVal: string) {
    layoutStore.value.changeSkinLayout(newVal);
  }
});

const siteMode = computed({
  get() {
    return layoutStore.value.mode;
  },
  set(newVal: string) {
    layoutStore.value.changeSiteMode(newVal);
  }
});

const dir = computed({
  get() {
    return layoutStore.value.dir;
  },
  set(newVal: string) {
    layoutStore.value.changeDir(newVal);
  }
});

const layoutWidth = computed({
  get() {
    return layoutStore.value.layoutWidth;
  },
  set(newVal: string) {
    layoutStore.value.changeLayoutWidth(newVal);
  }
});

const sideBarSize = computed({
  get() {
    return layoutStore.value.sideBarSize;
  },
  set(newVal: string) {
    layoutStore.value.changeSideBarSize(newVal);
  }
});

const navType = computed({
  get() {
    return layoutStore.value.navType;
  },
  set(newVal: string) {
    layoutStore.value.changeNavType(newVal);
  }
});

const sideBarColor = computed({
  get() {
    return layoutStore.value.sideBarColor;
  },
  set(newVal: string) {
    // isDarkTopBarAndSidebar.value =
    //   newVal === TOP_BAR.DARK && newVal === SIDEBAR_COLOR.DARK;
    layoutStore.value.changeSideBarColor(newVal);
  }
});

const topBarColor = computed({
  get() {
    return layoutStore.value.topBarColor;
  },
  set(newVal: string) {
    // isDarkTopBarAndSidebar.value =
    //   newVal === TOP_BAR.DARK && newVal === SIDEBAR_COLOR.DARK;
    layoutStore.value.changeTopBarColor(newVal);
  }
});

const isDarkTopBarAndSidebar = ref(
  topBarColor.value === TOP_BAR.DARK &&
    sideBarColor.value === SIDEBAR_COLOR.DARK
);

watch(isDarkTopBarAndSidebar, (newVal: boolean) => {
  layoutStore.value.changeTopBarColor(newVal ? TOP_BAR.DARK : TOP_BAR.LIGHT);
  layoutStore.value.changeSideBarColor(newVal ? TOP_BAR.DARK : TOP_BAR.LIGHT);
});
</script>
<template>
  <simplebar class="h-full p-6 overflow-y-auto">
    <div>
      <h5 class="mb-3 underline capitalize text-15">Choose Layouts</h5>
      <div class="grid grid-cols-1 mb-5 gap-7 sm:grid-cols-2">
        <div class="relative" @click="layoutType = LAYOUTS.VERTICAL">
          <input
            v-model="layoutType"
            class="absolute size-4 border rounded-full appearance-none cursor-pointer ltr:right-2 rtl:left-2 top-2 vertical-menu-btn bg-slate-100 border-slate-300 checked:bg-custom-500 checked:border-custom-500 dark:bg-zink-400 dark:border-zink-500"
            type="radio"
            :value="LAYOUTS.VERTICAL"
          />
          <VerticalLabel />
          <h5 class="mt-2 text-center text-15">Vertical</h5>
        </div>
        <div class="relative" @click="layoutType = LAYOUTS.HORIZONTAL">
          <input
            v-model="layoutType"
            class="absolute size-4 border rounded-full appearance-none cursor-pointer ltr:right-2 rtl:left-2 top-2 vertical-menu-btn bg-slate-100 border-slate-300 checked:bg-custom-500 checked:border-custom-500 dark:bg-zink-400 dark:border-zink-500"
            type="radio"
            :value="LAYOUTS.HORIZONTAL"
          />
          <HorizontalLabel />

          <h5 class="mt-2 text-center text-15">Horizontal</h5>
        </div>
      </div>
      <div id="semi-dark">
        <div class="flex items-center">
          <div
            class="relative inline-block w-10 mr-2 align-middle transition duration-200 ease-in"
          >
            <input
              v-model="isDarkTopBarAndSidebar"
              type="checkbox"
              name="customDefaultSwitch"
              id="customDefaultSwitch"
              class="absolute block size-5 transition duration-300 ease-linear border-2 rounded-full appearance-none cursor-pointer border-slate-200 bg-white/80 peer/published checked:bg-white checked:right-0 checked:border-custom-500 arrow-none dark:border-zink-500 dark:bg-zink-500 dark:checked:bg-zink-400 checked:bg-none"
            />
            <label
              for="customDefaultSwitch"
              class="block h-5 overflow-hidden transition duration-300 ease-linear border rounded-full cursor-pointer border-slate-200 bg-slate-200 peer-checked/published:bg-custom-500 peer-checked/published:border-custom-500 dark:border-zink-500 dark:bg-zink-600"
            ></label>
          </div>
          <label
            for="customDefaultSwitch"
            class="inline-block text-base font-medium"
            >Semi Dark (Sidebar & Header)</label
          >
        </div>
      </div>
    </div>
    <div class="mt-6">
      <!-- data-skin="" -->
      <h5 class="mb-3 underline capitalize text-15">Skin Layouts</h5>
      <div class="grid grid-cols-1 mb-5 gap-7 sm:grid-cols-2">
        <div class="relative" @click="skinLayoutType = SKIN_LAYOUT.DEFAULT">
          <input
            v-model="skinLayoutType"
            class="absolute size-4 border rounded-full appearance-none cursor-pointer ltr:right-2 rtl:left-2 top-2 vertical-menu-btn bg-slate-100 border-slate-300 checked:bg-custom-500 checked:border-custom-500 dark:bg-zink-400 dark:border-zink-500"
            type="radio"
            :value="SKIN_LAYOUT.DEFAULT"
          />
          <SkinDefault />
          <h5 class="mt-2 text-center text-15">Default</h5>
        </div>

        <div class="relative" @click="skinLayoutType = SKIN_LAYOUT.BORDERED">
          <input
            v-model="skinLayoutType"
            class="absolute size-4 border rounded-full appearance-none cursor-pointer ltr:right-2 rtl:left-2 top-2 vertical-menu-btn bg-slate-100 border-slate-300 checked:bg-custom-500 checked:border-custom-500 dark:bg-zink-400 dark:border-zink-500"
            type="radio"
            :value="SKIN_LAYOUT.BORDERED"
          />
          <SkinBordered />
          <h5 class="mt-2 text-center text-15">Bordered</h5>
        </div>
      </div>
    </div>
    <div class="mt-6">
      <!-- data-mode="" -->
      <h5 class="mb-3 underline capitalize text-15">Light & Dark</h5>
      <div class="flex gap-3">
        <Button
          :value="SITE_MODE.LIGHT"
          :class="{
            active: siteMode === SITE_MODE.LIGHT
          }"
          @click="siteMode = SITE_MODE.LIGHT"
        >
          Light Mode
        </Button>
        <Button
          :value="SITE_MODE.DARK"
          :class="{
            active: siteMode === SITE_MODE.DARK
          }"
          @click="siteMode = SITE_MODE.DARK"
        >
          Dark Mode
        </Button>
      </div>
    </div>
    <div class="mt-6">
      <!-- dir="ltr" -->
      <h5 class="mb-3 underline capitalize text-15">LTR & RTL</h5>
      <div class="flex flex-wrap gap-3">
        <Button
          :value="DIR.LTR"
          :class="{
            active: dir === DIR.LTR
          }"
          @click="dir = DIR.LTR"
        >
          LTR Mode
        </Button>
        <Button
          :value="DIR.RTL"
          :class="{
            active: dir === DIR.RTL
          }"
          @click="dir = DIR.RTL"
        >
          RTL Mode
        </Button>
      </div>
    </div>
    <div class="mt-6">
      <!-- data-content -->
      <h5 class="mb-3 underline capitalize text-15">Content Width</h5>
      <div class="flex gap-3">
        <Button
          :value="LAYOUT_WIDTH.FLUID"
          :class="{
            active: layoutWidth === LAYOUT_WIDTH.FLUID
          }"
          @click="layoutWidth = LAYOUT_WIDTH.FLUID"
        >
          Fluid
        </Button>
        <Button
          :value="LAYOUT_WIDTH.BOXED"
          :class="{
            active: layoutWidth === LAYOUT_WIDTH.BOXED
          }"
          @click="layoutWidth = LAYOUT_WIDTH.BOXED"
        >
          Boxed
        </Button>
      </div>
    </div>
    <div v-if="isVerticalLayout" class="mt-6" id="sidebar-size">
      <!-- data-sidebar-size="" -->
      <h5 class="mb-3 underline capitalize text-15">Sidebar Size</h5>
      <div class="flex flex-wrap gap-3">
        <Button
          :value="SIDEBAR_SIZE.DEFAULT"
          :class="{
            active: sideBarSize === SIDEBAR_SIZE.DEFAULT
          }"
          @click="sideBarSize = SIDEBAR_SIZE.DEFAULT"
        >
          Default
        </Button>
        <Button
          :value="SIDEBAR_SIZE.COMPACT"
          :class="{
            active: sideBarSize === SIDEBAR_SIZE.COMPACT
          }"
          @click="sideBarSize = SIDEBAR_SIZE.COMPACT"
        >
          Compact
        </Button>
        <Button
          :value="SIDEBAR_SIZE.SMALL"
          :class="{
            active: sideBarSize === SIDEBAR_SIZE.SMALL
          }"
          @click="sideBarSize = SIDEBAR_SIZE.SMALL"
        >
          Small (Icon)
        </Button>
      </div>
    </div>
    <div class="mt-6">
      <!-- data-navbar="" -->
      <h5 class="mb-3 underline capitalize text-15">Navigation Type</h5>
      <div class="flex flex-wrap gap-3">
        <Button
          :value="NAV_BAR_TYPE.STICKY"
          :class="{ active: navType === NAV_BAR_TYPE.STICKY }"
          @click="navType = NAV_BAR_TYPE.STICKY"
        >
          Sticky
        </Button>
        <Button
          :value="NAV_BAR_TYPE.SCROLL"
          :class="{ active: navType === NAV_BAR_TYPE.SCROLL }"
          @click="navType = NAV_BAR_TYPE.SCROLL"
        >
          Scroll
        </Button>
        <Button
          :value="NAV_BAR_TYPE.BORDERED"
          :class="{ active: navType === NAV_BAR_TYPE.BORDERED }"
          @click="navType = NAV_BAR_TYPE.BORDERED"
        >
          Bordered
        </Button>
        <Button
          :value="NAV_BAR_TYPE.HIDDEN"
          :class="{ active: navType === NAV_BAR_TYPE.HIDDEN }"
          @click="navType = NAV_BAR_TYPE.HIDDEN"
        >
          Hidden
        </Button>
      </div>
    </div>
    <div v-if="isVerticalLayout" class="mt-6" id="sidebar-color">
      <!-- data-sidebar="" light, dark, brand, modern -->
      <h5 class="mb-3 underline capitalize text-15">Sidebar Colors</h5>
      <div class="flex flex-wrap gap-3">
        <button
          :value="SIDEBAR_COLOR.LIGHT"
          class="flex items-center justify-center size-10 bg-white border rounded-md border-slate-200 group"
          :class="{
            active: sideBarColor === SIDEBAR_COLOR.LIGHT
          }"
          @click="sideBarColor = SIDEBAR_COLOR.LIGHT"
        >
          <Check
            class="size-5 hidden group-[.active]:inline-block text-slate-600"
          />
        </button>
        <button
          :value="SIDEBAR_COLOR.DARK"
          class="flex items-center justify-center size-10 border rounded-md border-zink-900 bg-zink-900 group"
          :class="{
            active: sideBarColor === SIDEBAR_COLOR.DARK
          }"
          @click="sideBarColor = SIDEBAR_COLOR.DARK"
        >
          <Check
            class="size-5 hidden group-[.active]:inline-block text-white"
          />
        </button>
        <button
          :value="SIDEBAR_COLOR.BRAND"
          class="flex items-center justify-center size-10 border rounded-md border-custom-800 bg-custom-800 group"
          :class="{
            active: sideBarColor === SIDEBAR_COLOR.BRAND
          }"
          @click="sideBarColor = SIDEBAR_COLOR.BRAND"
        >
          <Check
            class="size-5 hidden group-[.active]:inline-block text-white"
          />
        </button>
        <button
          :value="SIDEBAR_COLOR.MODERN"
          class="flex items-center justify-center size-10 border rounded-md border-purple-950 bg-gradient-to-t from-red-400 to-purple-500 group"
          :class="{
            active: sideBarColor === SIDEBAR_COLOR.MODERN
          }"
          @click="sideBarColor = SIDEBAR_COLOR.MODERN"
        >
          <Check
            class="size-5 hidden group-[.active]:inline-block text-white"
          />
        </button>
      </div>
    </div>
    <div class="mt-6">
      <!-- data-topbar="" light, dark, brand, modern -->
      <h5 class="mb-3 underline capitalize text-15">Topbar Colors</h5>
      <div class="flex flex-wrap gap-3">
        <button
          :value="TOP_BAR.LIGHT"
          :class="{
            active: topBarColor === TOP_BAR.LIGHT
          }"
          class="flex items-center justify-center size-10 bg-white border rounded-md border-slate-200 group"
          @click="topBarColor = TOP_BAR.LIGHT"
        >
          <Check
            class="size-5 hidden group-[.active]:inline-block text-slate-600"
          />
        </button>
        <button
          :value="TOP_BAR.DARK"
          :class="{
            active: topBarColor === TOP_BAR.DARK
          }"
          class="flex items-center justify-center size-10 border rounded-md border-zink-900 bg-zink-900 group"
          @click="topBarColor = TOP_BAR.DARK"
        >
          <Check
            class="size-5 hidden group-[.active]:inline-block text-white"
          />
        </button>
        <button
          :value="TOP_BAR.BRAND"
          :class="{
            active: topBarColor === TOP_BAR.BRAND
          }"
          class="flex items-center justify-center size-10 border rounded-md border-custom-800 bg-custom-800 group"
          @click="topBarColor = TOP_BAR.BRAND"
        >
          <Check
            class="size-5 hidden group-[.active]:inline-block text-white"
          />
        </button>
      </div>
    </div>
  </simplebar>
  <div
    class="flex items-center justify-between gap-3 p-4 border-t border-slate-200 dark:border-zink-500"
  >
    <button
      type="button"
      class="w-full transition-all duration-200 ease-linear text-slate-500 btn bg-slate-200 border-slate-200 hover:text-slate-600 hover:bg-slate-300 hover:border-slate-300 focus:text-slate-600 focus:bg-slate-300 focus:border-slate-300 focus:ring focus:ring-slate-100"
    >
      Reset
    </button>
    <a
      href="#!"
      class="w-full text-white transition-all duration-200 ease-linear bg-red-500 border-red-500 btn hover:text-white hover:bg-red-600 hover:border-red-600 focus:text-white focus:bg-red-600 focus:border-red-600 focus:ring focus:ring-red-100 active:text-white active:bg-red-600 active:border-red-600 active:ring active:ring-red-100"
      >Buy Now</a
    >
  </div>
</template>
