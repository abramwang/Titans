<script lang="ts" setup>
import { ref, computed, onMounted, onBeforeUnmount, watch } from "vue";
import { ChevronsLeft, ChevronsRight, Search } from "lucide-vue-next";
import { logoDark, logoLight, logoSm } from "@/assets/images/utils";
import Language from "@/app/layout/navbar/Language.vue";
import SiteMode from "@/app/layout/navbar/SiteMode.vue";
import Cart from "@/app/layout/navbar/Cart.vue";
import Notification from "@/app/layout/navbar/Notification.vue";
import Settings from "@/app/layout/navbar/Settings.vue";
import Profile from "@/app/layout/navbar/Profile.vue";
import CartDrawer from "@/app/layout/navbar/CartDrawer.vue";
import CustomizerDrawer from "@/app/layout/navbar/customizer/Drawer.vue";
import { useLayoutStore } from "@/store/layout";
import { SIDEBAR_SIZE } from "@/app/const";

const cardDrawer = ref(false);
const cartCount = ref(0);
const isVisible = ref(false);

const customizerDrawer = ref(false);

const layoutStore = computed(() => {
  return useLayoutStore();
});

const sideBarSize = computed(() => {
  return layoutStore.value.sideBarSize;
});

const clientWidth = ref(document.documentElement.clientWidth);

onMounted(() => {
  window.addEventListener("resize", onWindowResize);
});

onBeforeUnmount(() => {
  window.removeEventListener("resize", onWindowResize);
});

const onWindowResize = () => {
  clientWidth.value = document.documentElement.clientWidth;
  isVisible.value = false;
  if (clientWidth.value < 768) {
    const { DEFAULT } = SIDEBAR_SIZE;

    layoutStore.value.changeSideBarSize(DEFAULT);
  }
};
const toggleCartDrawer = () => {
  cardDrawer.value = !cardDrawer.value;
};

const toggleCustomizerDrawer = () => {
  customizerDrawer.value = !customizerDrawer.value;
};

const updateCount = (newVal: number) => {
  cartCount.value = newVal;
};

watch(isVisible, (newVal: boolean) => {
  if (newVal) {
    document.body.classList.add("overflow-hidden");
  } else {
    document.body.classList.remove("overflow-hidden");
  }
});

const toggleLeftSideBar = () => {
  const { DEFAULT, SMALL } = SIDEBAR_SIZE;
  if (clientWidth.value >= 768) {
    if (sideBarSize.value === DEFAULT) {
      layoutStore.value.changeSideBarSize(SMALL);
    } else {
      layoutStore.value.changeSideBarSize(DEFAULT);
    }
  } else {
    layoutStore.value.changeSideBarSize(DEFAULT);
    const appMenu = document.querySelector(".app-menu");
    if (appMenu) {
      isVisible.value = !isVisible.value;
      appMenu.classList.remove("hidden");
    }
  }
};

const onOverlayClick = () => {
  const appMenu = document.querySelector(".app-menu");
  if (appMenu) {
    appMenu.classList.add("hidden");
  }
  isVisible.value = false;
};
</script>
<template>
  <Teleport to="body">
    <div
      v-if="isVisible"
      class="fixed inset-0 bg-slate-900/40 dark:bg-zink-800/70 z-[1049] backdrop-overlay"
      @click="onOverlayClick"
    ></div>
  </Teleport>
  <header
    id="page-topbar"
    class="ltr:md:left-vertical-menu rtl:md:right-vertical-menu group-data-[sidebar-size=md]:ltr:md:left-vertical-menu-md group-data-[sidebar-size=md]:rtl:md:right-vertical-menu-md group-data-[sidebar-size=sm]:ltr:md:left-vertical-menu-sm group-data-[sidebar-size=sm]:rtl:md:right-vertical-menu-sm group-data-[layout=horizontal]:ltr:left-0 group-data-[layout=horizontal]:rtl:right-0 fixed right-0 z-[1000] left-0 print:hidden group-data-[navbar=bordered]:m-4 group-data-[navbar=bordered]:[&.is-sticky]:mt-0 transition-all ease-linear duration-300 group-data-[navbar=hidden]:hidden group-data-[navbar=scroll]:absolute group/topbar group-data-[layout=horizontal]:z-drawer group-data-[layout=horizontal]:md:z-[1000]"
  >
    <!-- group-data-[layout=horizontal]:z-[1004] -->
    <div class="layout-width">
      <div
        class="flex items-center px-4 mx-auto bg-topbar border-b-2 border-topbar group-data-[topbar=dark]:bg-topbar-dark group-data-[topbar=dark]:border-topbar-dark group-data-[topbar=brand]:bg-topbar-brand group-data-[topbar=brand]:border-topbar-brand shadow-md h-header shadow-slate-200/50 group-data-[navbar=bordered]:rounded-md group-data-[navbar=bordered]:group-[.is-sticky]/topbar:rounded-t-none group-data-[topbar=dark]:dark:bg-zink-700 group-data-[topbar=dark]:dark:border-zink-700 dark:shadow-none group-data-[topbar=dark]:group-[.is-sticky]/topbar:dark:shadow-zink-500 group-data-[topbar=dark]:group-[.is-sticky]/topbar:dark:shadow-md group-data-[navbar=bordered]:shadow-none group-data-[layout=horizontal]:group-data-[navbar=bordered]:rounded-b-none group-data-[layout=horizontal]:shadow-none group-data-[layout=horizontal]:dark:group-[.is-sticky]/topbar:shadow-none"
      >
        <div
          class="flex items-center w-full group-data-[layout=horizontal]:mx-auto group-data-[layout=horizontal]:max-w-screen-2xl navbar-header group-data-[layout=horizontal]:ltr:xl:pr-3 group-data-[layout=horizontal]:rtl:xl:pl-3"
        >
          <div
            class="items-center justify-center hidden px-5 text-center h-header group-data-[layout=horizontal]:md:flex group-data-[layout=horizontal]:ltr::pl-0 group-data-[layout=horizontal]:rtl:pr-0"
          >
            <router-link to="/">
              <span class="hidden">
                <img :src="logoSm" alt="" class="h-6 mx-auto" />
              </span>
              <span
                class="group-data-[topbar=dark]:hidden group-data-[topbar=brand]:hidden"
              >
                <img :src="logoDark" alt="" class="h-6 mx-auto" />
              </span>
            </router-link>
            <router-link
              to="/"
              class="hidden group-data-[topbar=dark]:block group-data-[topbar=brand]:block"
            >
              <span
                class="group-data-[topbar=dark]:hidden group-data-[topbar=brand]:hidden"
              >
                <img :src="logoSm" alt="" class="h-6 mx-auto" />
              </span>
              <span
                class="group-data-[topbar=dark]:block group-data-[topbar=brand]:block"
              >
                <img :src="logoLight" alt="" class="h-6 mx-auto" />
              </span>
            </router-link>
          </div>

          <button
            type="button"
            class="inline-flex relative justify-center items-center p-0 text-topbar-item transition-all w-[37.5px] h-[37.5px] duration-75 ease-linear bg-topbar rounded-md btn hover:bg-slate-100 group-data-[topbar=dark]:bg-topbar-dark group-data-[topbar=dark]:border-topbar-dark group-data-[topbar=dark]:text-topbar-item-dark group-data-[topbar=dark]:hover:bg-topbar-item-bg-hover-dark group-data-[topbar=dark]:hover:text-topbar-item-hover-dark first-letter: group-data-[topbar=brand]:bg-topbar-brand group-data-[topbar=brand]:border-topbar-brand group-data-[topbar=brand]:text-topbar-item-brand group-data-[topbar=brand]:hover:bg-topbar-item-bg-hover-brand group-data-[topbar=brand]:hover:text-topbar-item-hover-brand group-data-[topbar=dark]:dark:bg-zink-700 group-data-[topbar=dark]:dark:text-zink-200 group-data-[topbar=dark]:dark:border-zink-700 group-data-[topbar=dark]:dark:hover:bg-zink-600 group-data-[topbar=dark]:dark:hover:text-zink-50 group-data-[layout=horizontal]:flex group-data-[layout=horizontal]:md:hidden hamburger-icon"
            id="topnav-hamburger-icon"
            @click="toggleLeftSideBar"
          >
            <ChevronsLeft
              v-if="sideBarSize === SIDEBAR_SIZE.DEFAULT"
              class="size-5"
            />
            <ChevronsRight v-else class="size-5" />
          </button>

          <div
            class="relative hidden rtl:mr-3 lg:block group-data-[layout=horizontal]:hidden group-data-[layout=horizontal]:lg:block ltr:ml-3"
          >
            <input
              type="text"
              class="py-2 pr-4 text-sm text-topbar-item bg-topbar border border-topbar-border rounded pl-8 placeholder:text-slate-400 form-control focus-visible:outline-0 min-w-[300px] focus:border-blue-400 group-data-[topbar=dark]:bg-topbar-dark group-data-[topbar=dark]:border-topbar-border-dark group-data-[topbar=dark]:placeholder:text-slate-500 group-data-[topbar=dark]:text-topbar-item-dark group-data-[topbar=brand]:bg-topbar-brand group-data-[topbar=brand]:border-topbar-border-brand group-data-[topbar=brand]:placeholder:text-blue-300 group-data-[topbar=brand]:text-topbar-item-brand group-data-[topbar=dark]:dark:bg-zink-700 group-data-[topbar=dark]:dark:border-zink-500 group-data-[topbar=dark]:dark:text-zink-100"
              placeholder="Search for ..."
              autocomplete="off"
            />

            <Search
              class="inline-block size-4 absolute left-2.5 top-2.5 text-topbar-item fill-slate-100 group-data-[topbar=dark]:fill-topbar-item-bg-hover-dark group-data-[topbar=dark]:text-topbar-item-dark group-data-[topbar=brand]:fill-topbar-item-bg-hover-brand group-data-[topbar=brand]:text-topbar-item-brand group-data-[topbar=dark]:dark:text-zink-200 group-data-[topbar=dark]:dark:fill-zink-600"
            />
          </div>

          <div class="flex gap-3 ms-auto">
            <div class="relative flex items-center dropdown h-header">
              <Language />
            </div>
            <SiteMode />
            <Cart @toggleDrawer="toggleCartDrawer" />
            <Notification />
            <div class="relative items-center hidden h-header md:flex">
              <Settings @toggleDrawer="toggleCustomizerDrawer" />
            </div>
            <Profile />
          </div>
        </div>
      </div>
    </div>
  </header>
  <TDrawer v-model="cardDrawer" width="md:w-96">
    <template #title>
      <div class="grow">
        <h5 class="mb-0 text-16 font-medium">
          Shopping Cart
          <span
            class="inline-flex items-center justify-center size-5 ml-1 text-[11px] font-medium border rounded-full text-white bg-custom-500 border-custom-500"
          >
            {{ cartCount }}
          </span>
        </h5>
      </div>
    </template>
    <template #content>
      <CartDrawer @updateCount="updateCount" @hideCart="cardDrawer = false" />
    </template>
  </TDrawer>
  <TDrawer v-model="customizerDrawer" width="md:w-96">
    <template #title>
      <div class="flex justify-between border-slate-200 dark:border-zink-500">
        <div class="grow">
          <h5 class="mb-1 text-16">Tailwick Theme Customizer</h5>
          <p class="font-normal text-slate-500 dark:text-zink-200">
            Choose your themes & layouts etc.
          </p>
        </div>
      </div>
    </template>
    <template #content>
      <CustomizerDrawer v-if="customizerDrawer" />
    </template>
  </TDrawer>
</template>
