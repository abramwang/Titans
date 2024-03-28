<script lang="ts" setup>
import { computed } from "vue";
import { useRoute } from "vue-router";
import { useLayoutStore } from "@/store/layout";
import { LAYOUTS, SIDEBAR_SIZE } from "@/app/const";

const emit = defineEmits(["toggleActivation"]);
const route = useRoute();
const path = computed(() => route.path);
const layoutStore = useLayoutStore();

const layoutType = computed(() => layoutStore.layoutType);

const isHorizontal = computed(() => layoutType.value === LAYOUTS.HORIZONTAL);
const isSmallSidebar = computed(
  () => layoutStore.sideBarSize === SIDEBAR_SIZE.SMALL
);

const isDefaultSideBar = computed(
  () => layoutStore.sideBarSize === SIDEBAR_SIZE.DEFAULT
);

const props = defineProps({
  menuItem: {
    type: Object,
    default: () => {}
  },
  isNested: {
    type: Boolean,
    default: false
  }
});

const menuItemData = computed(() => props.menuItem);

const isDrawerActive = computed(() => {
  const isRouteAvailable = menuItemData.value.subMenu.some((item: any) => {
    if (item.subMenu) {
      const isActiveSubMenu = isNestedMenuActivated(item.subMenu);
      return isActiveSubMenu;
    }
    return item.path === path.value;
  });
  return isRouteAvailable;
});

const isNestedMenuActivated = (menuItems: any) => {
  return menuItems.some((menuItem: any) => {
    if (menuItem.path === path.value) {
      return true;
    } else if (menuItem.subMenu) {
      const path = isNestedMenuActivated(menuItem.subMenu);
      return path;
    }
    return false;
  });
};

const toggleActivation = (id: number) => {
  emit("toggleActivation", id);
};
</script>
<template>
  <li
    class="group-data-[layout=horizontal]:shrink-0"
    :class="{
      'group/sub group-data-[sidebar-size=sm]:relative': isNested,
      'group/sm': !isNested
    }"
  >
    <router-link
      class="relative dropdown-button flex items-center px-3 mx-3 group/menu-link text-vertical-menu-item-font-size font-normal transition-all duration-75 ease-linear rounded-md group-data-[sidebar-size=sm]:ltr:rounded-br-none group-data-[sidebar-size=sm]:rtl:rounded-bl-none py-2.5 text-vertical-menu-item hover:text-vertical-menu-item-hover hover:bg-vertical-menu-item-bg-hover [&.active]:text-vertical-menu-item-active group-data-[sidebar=dark]:text-vertical-menu-item-dark group-data-[sidebar=dark]:hover:text-vertical-menu-item-hover-dark group-data-[sidebar=dark]:dark:hover:text-custom-500 group-data-[layout=horizontal]:dark:hover:text-custom-500 group-data-[sidebar=dark]:hover:bg-vertical-menu-item-bg-hover-dark group-data-[sidebar=dark]:dark:hover:bg-zink-600 group-data-[sidebar=dark]:[&.active]:text-vertical-menu-item-active-dark group-data-[sidebar=brand]:text-vertical-menu-item-brand group-data-[sidebar=brand]:hover:text-vertical-menu-item-hover-brand group-data-[sidebar=brand]:hover:bg-vertical-menu-item-bg-hover-brand group-data-[sidebar=brand]:[&.active]:text-vertical-menu-item-active-brand group-data-[sidebar=modern]:text-vertical-menu-item-modern group-data-[sidebar=modern]:hover:bg-vertical-menu-item-bg-hover-modern group-data-[sidebar=modern]:hover:text-vertical-menu-item-hover-modern group-data-[sidebar=modern]:[&.active]:text-vertical-menu-item-active-modern group-data-[sidebar-size=md]:block group-data-[sidebar-size=md]:text-center group-data-[sidebar-size=sm]:group-hover/sm:bg-vertical-menu group-data-[sidebar-size=sm]:group-data-[sidebar=brand]:group-hover/sm:bg-vertical-menu-brand group-data-[sidebar-size=sm]:my-0 group-data-[layout=horizontal]:m-0 group-data-[layout=horizontal]:ltr:pr-8 group-data-[layout=horizontal]:rtl:pl-8 group-data-[layout=horizontal]:hover:bg-transparent group-data-[layout=horizontal]:[&.active]:bg-transparent [&.dropdown-button]:before:absolute [&.dropdown-button]:[&.show]:before:content-['\ea4e'] [&.dropdown-button]:before:content-['\ea6e'] [&.dropdown-button]:before:font-remix ltr:[&.dropdown-button]:before:right-2 rtl:[&.dropdown-button]:before:left-2 [&.dropdown-button]:before:text-16 group-data-[sidebar-size=sm]:[&.dropdown-button]:before:hidden group-data-[sidebar-size=md]:[&.dropdown-button]:before:hidden group-data-[sidebar=dark]:dark:text-zink-200 group-data-[layout=horizontal]:dark:text-zink-200 group-data-[layout=horizontal]:dark:[&.active]:text-custom-500 rtl:[&.dropdown-button]:before:rotate-180 group-data-[layout=horizontal]:[&.dropdown-button]:[&.show]:before:rotate-0 rtl:[&.dropdown-button]:[&.show]:before:rotate-0 group-data-[sidebar-size=sm]:group-data-[sidebar=dark]:group-hover/sm:bg-vertical-menu-dark"
      to="#!"
      :class="{
        active: isDrawerActive,
        show: isDrawerActive || menuItemData.isActive,
        'group-data-[layout=horizontal]:md:px-3': isNested && menuItemData.icon,
        'group-data-[layout=horizontal]:md:px-5':
          isNested && !menuItemData.icon,
        '[&.active]:bg-vertical-menu-item-bg-active group-data-[sidebar=dark]:[&.active]:bg-vertical-menu-item-bg-active-dark group-data-[sidebar=brand]:[&.active]:bg-vertical-menu-item-bg-active-brand group-data-[sidebar=modern]:[&.active]:bg-vertical-menu-item-bg-active-modern group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:group-hover/sm:bg-vertical-menu-border-modern':
          !isNested,
        'after:absolute ltr:after:left-1.5 rtl:after:right-1.5 after:top-4.5 after:size-1 after:rounded after:transition-all after:duration-75 after:ease-linear after:bg-vertical-menu-sub-item hover:after:bg-vertical-menu-sub-item-hover [&.active]:after:bg-vertical-menu-sub-item-active ':
          !isHorizontal && isDefaultSideBar && isNested,
        'my-1 mx-3': !isNested,
        'ltr:pl-6 rtl:pr-6':
          isNested && menuItemData.subMenu.length && isDefaultSideBar,
        'ltr:mr-3 rtl:ml-3': isNested,
        'group-data-[sidebar-size=sm]:group-hover/sm:w-[calc(theme(spacing.vertical-menu-sm)_*_3.62)]':
          !isNested, // title of submenu on hover / click,
        'group-data-[sidebar-size=sm]:[&.dropdown-button]:after:hidden group-data-[layout=horizontal]:[&.dropdown-button]:before:rotate-90':
          !isNested,
        'ml-0': isNested && isDefaultSideBar,
        'group-data-[sidebar-size=sm]:group-data-[sidebar=dark]:group-hover/sm:!bg-transparent group-data-[sidebar=modern]:group-data-[sidebar-size=sm]:group-hover/sm:bg-transparent group-data-[layout=horizontal]:[&.dropdown-button]:before:rotate-270':
          isNested
      }"
      @click.prevent="toggleActivation(menuItemData.id)"
    >
      <!-- relative -->
      <span
        v-if="menuItemData.icon"
        class="min-w-[1.75rem] group-data-[sidebar-size=sm]:h-[1.75rem] inline-block text-start text-[16px] group-data-[sidebar-size=md]:block group-data-[sidebar-size=sm]:flex group-data-[sidebar-size=sm]:items-center"
      >
        <component
          :is="menuItemData.icon"
          class="h-4 group-data-[sidebar-size=sm]:h-5 group-data-[sidebar-size=sm]:w-5 transition group-hover/menu-link:animate-icons fill-slate-100 group-hover/menu-link:fill-blue-200 group-data-[sidebar=dark]:fill-vertical-menu-item-bg-active-dark group-data-[sidebar=dark]:dark:fill-zink-600 group-data-[layout=horizontal]:dark:fill-zink-600 group-data-[sidebar=brand]:fill-vertical-menu-item-bg-active-brand group-data-[sidebar=modern]:fill-vertical-menu-item-bg-active-modern group-data-[sidebar=dark]:group-hover/menu-link:fill-vertical-menu-item-bg-active-dark group-data-[sidebar=dark]:group-hover/menu-link:dark:fill-custom-500/20 group-data-[layout=horizontal]:dark:group-hover/menu-link:fill-custom-500/20 group-data-[sidebar=brand]:group-hover/menu-link:fill-vertical-menu-item-bg-active-brand group-data-[sidebar=modern]:group-hover/menu-link:fill-vertical-menu-item-bg-active-modern group-data-[sidebar-size=md]:block group-data-[sidebar-size=md]:mx-auto group-data-[sidebar-size=md]:mb-2"
        />
      </span>
      <!--  -->
      <span
        class="align-middle group-data-[sidebar-size=sm]:hidden group-data-[sidebar-size=sm]:group-hover/sm:block"
        :data-key="`t-${menuItemData.title}`"
        :class="{
          'group-data-[sidebar-size=sm]:ltr:pl-10 group-data-[sidebar-size=sm]:rtl:pr-10':
            !isNested
        }"
        style="text-wrap: nowrap"
      >
        {{ $t(`t-${menuItemData.title}`) }}
      </span>
    </router-link>
    <div
      class="dropdown-content group-data-[sidebar-size=sm]:ltr:left-full group-data-[sidebar-size=sm]:rtl:right-full group-data-[layout=horizontal]:z-10 group-data-[sidebar-size=sm]:w-[calc(theme('spacing.vertical-menu-sm')_*_2.8)] group-data-[sidebar-size=sm]:absolute bg-vertical-menu group-data-[sidebar=dark]:bg-vertical-menu-dark group-data-[sidebar=dark]:dark:bg-zink-700 group-data-[sidebar=brand]:bg-vertical-menu-brand group-data-[sidebar=modern]:bg-transparent group-data-[layout=horizontal]:md:w-44 group-data-[layout=horizontal]:py-2 group-data-[layout=horizontal]:rounded-b-md group-data-[layout=horizontal]:md:shadow-lg group-data-[layout=horizontal]:md:shadow-slate-500/10 group-data-[layout=horizontal]:dark:bg-zink-700 group-data-[layout=horizontal]:dark:md:shadow-zink-600/20 group-data-[sidebar-size=sm]:rounded-br-md group-data-[sidebar-size=sm]:shadow-lg group-data-[sidebar-size=sm]:shadow-slate-700/10 group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:group-hover/sm:to-vertical-menu-to-modern group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:group-hover/sm:from-vertical-menu-from-modern group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:group-hover/sm:bg-gradient-to-tr group-data-[sidebar-size=sm]:rounded-b-sm group-data-[sidebar-size=sm]:hidden group-data-[sidebar-size=sm]:group-hover/sub:block"
      :class="{
        hidden:
          !isHorizontal && !isSmallSidebar
            ? !isDrawerActive && !menuItemData.isActive
            : !menuItemData.isActive,
        show: isDrawerActive || menuItemData.isActive,
        'group-data-[layout=horizontal]:ltr:md:left-full group-data-[layout=horizontal]:rtl:md:right-full group-data-[layout=horizontal]:md:absolute group-data-[sidebar-size=sm]:rounded-tr-md group-data-[sidebar-size=sm]:top-0':
          isNested,
        'group-data-[layout=horizontal]:md:fixed': !isNested,
        'group-data-[sidebar-size=sm]:hidden group-data-[sidebar-size=sm]:group-hover/sm:block group-data-[sidebar-size=sm]:top-auto':
          !isNested // disable hover effect on nested submenu,
      }"
    >
      <ul
        class="ltr:pl-[1.75rem] rtl:pr-[1.75rem] group-data-[sidebar-size=md]:ltr:pl-0 group-data-[sidebar-size=md]:rtl:pr-0 group-data-[sidebar-size=sm]:ltr:pl-0 group-data-[sidebar-size=sm]:rtl:pr-0 group-data-[sidebar-size=sm]:py-1 group-data-[layout=horizontal]:ltr:pl-0 group-data-[layout=horizontal]:rtl:pr-0"
        :class="{
          'group-data-[layout=horizontal]:max-h-80 group-data-[layout=horizontal]:overflow-y-auto':
            isNested
        }"
      >
        <!-- group-data-[layout=horizontal]:relative -->
        <template v-for="subMenu in menuItemData.subMenu">
          <li
            v-if="!subMenu.subMenu && subMenu.path"
            @click="toggleActivation(subMenu.id)"
          >
            <router-link
              :to="subMenu.path"
              class="relative flex items-center px-6 py-2 text-vertical-menu-item-font-size transition-all duration-75 ease-linear text-vertical-menu-sub-item hover:text-vertical-menu-sub-item-hover [&.active]:text-vertical-menu-sub-item-active before:absolute ltr:before:left-1.5 rtl:before:right-1.5 before:top-4 before:w-1 before:h-1 before:rounded before:transition-all before:duration-75 before:ease-linear before:bg-vertical-menu-sub-item hover:before:bg-vertical-menu-sub-item-hover [&.active]:before:bg-vertical-menu-sub-item-active group-data-[sidebar=dark]:text-vertical-menu-sub-item-dark group-data-[sidebar=dark]:dark:text-zink-200 group-data-[layout=horizontal]:dark:text-zink-200 group-data-[layout=horizontal]:dark:hover:text-custom-500 group-data-[sidebar=dark]:hover:text-vertical-menu-sub-item-hover-dark group-data-[sidebar=dark]:dark:hover:text-custom-500 group-data-[sidebar=dark]:[&.active]:text-vertical-menu-sub-item-active-dark group-data-[sidebar=dark]:dark:[&.active]:text-custom-500 group-data-[layout=horizontal]:dark:[&.active]:text-custom-500 group-data-[sidebar=dark]:before:bg-vertical-menu-sub-item-dark/50 group-data-[sidebar=dark]:hover:before:bg-vertical-menu-sub-item-hover-dark group-data-[sidebar=dark]:dark:hover:before:bg-custom-500 group-data-[sidebar=dark]:[&.active]:before:bg-vertical-menu-sub-item-active-dark group-data-[sidebar=dark]:dark:[&.active]:before:bg-custom-500 group-data-[sidebar=brand]:text-vertical-menu-sub-item-brand group-data-[sidebar=brand]:hover:text-vertical-menu-sub-item-hover-brand group-data-[sidebar=brand]:before:bg-vertical-menu-sub-item-brand/80 group-data-[sidebar=brand]:hover:before:bg-vertical-menu-sub-item-hover-brand/80 group-data-[sidebar=brand]:[&.active]:before:bg-vertical-menu-sub-item-active-brand/80 group-data-[sidebar=brand]:[&.active]:text-vertical-menu-sub-item-active-brand group-data-[sidebar=modern]:text-vertical-menu-sub-item-modern group-data-[sidebar=modern]:before:bg-vertical-menu-sub-item-modern/70 group-data-[sidebar=modern]:hover:text-vertical-menu-sub-item-hover-modern group-data-[sidebar=modern]:before:hover:bg-vertical-menu-sub-item-hover-modern group-data-[sidebar=modern]:[&.active]:text-vertical-menu-sub-item-active-modern group-data-[sidebar=modern]:before:[&.active]:text-vertical-menu-sub-item-active-modern group-data-[sidebar-size=md]:before:hidden group-data-[sidebar-size=md]:text-center group-data-[sidebar-size=md]:block group-data-[sidebar-size=sm]:before:hidden group-data-[layout=horizontal]:before:left-[1.4rem] group-data-[layout=horizontal]:md:before:hidden group-data-[layout=horizontal]:ltr:pl-10 group-data-[layout=horizontal]:rtl:pr-10 group-data-[layout=horizontal]:ltr:pr-5 group-data-[layout=horizontal]:rtl:pl-5 group-data-[layout=horizontal]:md:!px-5"
              :class="{
                active: subMenu.path === path
              }"
              :data-key="`t-${subMenu.title}`"
            >
              {{ $t(`t-${subMenu.title}`) }}
            </router-link>
          </li>
          <SubMenu
            v-else
            :menu-item="subMenu"
            is-nested
            @toggleActivation="toggleActivation"
          />
        </template>
      </ul>
    </div>
  </li>
</template>
