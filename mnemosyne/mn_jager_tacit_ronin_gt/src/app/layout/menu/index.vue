<script lang="ts" setup>
import { computed, ref, onMounted, watch, onBeforeUnmount } from "vue";
import { useRoute, useRouter } from "vue-router";
import { logoDark, logoLight, logoSm } from "@/assets/images/utils";
import { menuItems } from "@/app/layout/utils";
import SubMenu from "@/app/layout/menu/SubMenu.vue";
import { useLayoutStore } from "@/store/layout";
import { LucideNetwork } from "lucide-vue-next";
import { LAYOUTS, SIDEBAR_SIZE } from "@/app/const";
import { v4 as uuidv4 } from "uuid";
import { MenuItemType, SubMenuType } from "@/app/layout/types";

const layoutStore = computed(() => useLayoutStore());
const layoutType = computed(() => layoutStore.value.layoutType);
const isHorizontal = computed(() => layoutType.value === LAYOUTS.HORIZONTAL);

const route = useRoute();
const router = useRouter();
const path = computed(() => route.path);

const sideBarComponent = computed(() => {
  const sideBarSize = layoutStore.value.sideBarSize;

  if (sideBarSize === SIDEBAR_SIZE.SMALL) {
    return "div";
  } else {
    return "simplebar";
  }
});
const onLogoClick = () => {
  router.push("/");
};
const mappedData: any = menuItems.map((item) => {
  if (item.subMenu) {
    const nestedSubMenu = item.subMenu.map((subMenu: SubMenuType) => {
      if (subMenu.subMenu) {
        return {
          ...subMenu,
          isActive: false,
          id: uuidv4()
        };
      }
      return {
        ...subMenu
      };
    });

    return {
      ...item,
      subMenu: nestedSubMenu,
      isActive: false,
      id: uuidv4()
    };
  }
  return {
    ...item,
    id: uuidv4()
  };
});

const clientWidth = ref(document.documentElement.clientWidth);

const menuItemData = ref<MenuItemType[]>(mappedData);

const onWindowResize = () => {
  clientWidth.value = document.documentElement.clientWidth;
  if (isHorizontal.value) {
    setTimeout(() => {
      setupHorizontalMenu();
    }, 300);
  } else {
    menuItemData.value = mappedData;
  }
};

onMounted(() => {
  window.addEventListener("resize", onWindowResize);

  if (isHorizontal.value) {
    setTimeout(() => {
      setupHorizontalMenu();
    }, 300);
  }
  document.documentElement.addEventListener("click", (event: any) => {
    isSubMenu(event.target);
  });
});

onBeforeUnmount(() => {
  document.documentElement.removeEventListener("click", (event: any) => {
    isSubMenu(event.target);
  });
  window.removeEventListener("resize", setupHorizontalMenu);
  window.removeEventListener("resize", onWindowResize);
});

const isSubMenu = (element: HTMLElement | null): boolean => {
  if (!element) {
    hideActivation();
    return false;
  }
  return (
    element.classList.contains("submenu-dropdown") ||
    element.classList.contains("app-menu") ||
    isSubMenu(element.parentElement)
  );
};

watch(layoutType, (newVal: string) => {
  if (newVal === LAYOUTS.HORIZONTAL) {
    setupHorizontalMenu();
  } else {
    menuItemData.value = mappedData;
  }
});

const setupHorizontalMenu = () => {
  if (clientWidth.value >= 768) {
    window.addEventListener("resize", setupHorizontalMenu);

    let isMoreMenu = false;
    const navData = mappedData.filter(
      (menuItem: MenuItemType) => !menuItem.isHeader
    );
    const horizontalMenuEle = document.querySelector(".app-menu");
    const navbarNav = document.querySelector(".navbar-header");
    if (navbarNav) {
      const fullMenuWidth = navbarNav.clientWidth - 100 || 0;

      let totalItemsWidth = 0;
      let visibleItems = [];
      let hiddenItems = [];

      for (let i = 0; i < navData.length; i++) {
        const element: HTMLElement = horizontalMenuEle?.querySelector(
          "#navbar-nav"
        )?.children[i] as HTMLElement;
        const itemWidth = element?.offsetWidth || 0;
        totalItemsWidth += itemWidth;

        if (totalItemsWidth + 250 <= fullMenuWidth) {
          visibleItems.push(navData[i]);
        } else {
          hiddenItems.push(navData[i]);
        }
      }

      const moreMenuItem = {
        title: "more",
        icon: LucideNetwork,
        isActive: false,
        subMenu: hiddenItems,
        id: uuidv4(),
        stateVariables: isMoreMenu,
        click: (e: any) => {
          e.preventDefault();
          isMoreMenu = !isMoreMenu;
        }
      };

      let updatedMenuItems: any = [...visibleItems];
      if (hiddenItems.length) {
        updatedMenuItems.push(moreMenuItem);
      }

      menuItemData.value = updatedMenuItems;
    }
    setTimeout(() => {
      handleDropdownMenu();
    }, 500);
  } else {
    menuItemData.value = mappedData;
  }
};
const toggleActivation = (menuItemId: string) => {
  menuItemData.value = menuItemData.value.map((item: MenuItemType) => {
    if (item.id === menuItemId) {
      return {
        ...item,
        isActive: !item.isActive
      };
    } else if (item.subMenu) {
      const nestedSubmenu = getActivations(menuItemId, item);
      const firstLevelMenu = {
        ...item,
        isActive: nestedSubmenu.some(
          (subMenu: SubMenuType) => subMenu.isActive
        ),
        subMenu: nestedSubmenu
      };
      return firstLevelMenu;
    }
    return { ...item, isActive: false };
  });
};

const getActivations: any = (menuItemId: string, menuItem: MenuItemType) => {
  const preparedData = menuItem.subMenu?.map((subMenu: SubMenuType) => {
    if (menuItemId && subMenu.id === menuItemId) {
      return {
        ...subMenu,
        isActive: !subMenu.isActive
      };
    } else {
      if (subMenu.subMenu) {
        const nestedSubmenu = getActivations(menuItemId, subMenu);

        const temp = {
          ...subMenu,
          subMenu: nestedSubmenu,
          isActive: nestedSubmenu.some((item: SubMenuType) => item.isActive)
        };
        return temp;
      }
      return { ...subMenu, isActive: false };
    }
  });

  return preparedData;
};

const hideActivation = () => {
  menuItemData.value = menuItemData.value.map((item) => {
    if (item.subMenu) {
      const nestedSubmenu = item.subMenu.map((subMenu) => {
        return {
          ...subMenu,
          isActive: false
        };
      });
      return {
        ...item,
        isActive: false,
        subMenu: nestedSubmenu
      };
    }
    return { ...item, isActive: false };
  });
};

function handleDropdownMenu() {
  const dropdownToggleButtons = document
    .querySelector(".app-menu")
    ?.querySelectorAll(".dropdown-button");
  dropdownToggleButtons?.forEach((button) => {
    const content = button.nextElementSibling;
    button.addEventListener("click", () => {
      if (!content) {
        return;
      }
      setTimeout(() => {
        // get the dropdown menu element
        var dropdownMenu = button;
        const subMenus: any = dropdownMenu.nextElementSibling
          ? dropdownMenu.nextElementSibling
          : dropdownMenu.parentElement?.nextElementSibling;
        if (subMenus) {
          const isLeftFull = subMenus.classList.contains(
            "group-data-[layout=horizontal]:ltr:md:left-full"
          );

          const isRightFull = subMenus.classList.contains(
            "group-data-[layout=horizontal]:rtl:md:right-full"
          );
          if (isLeftFull || isRightFull) {
            if (dropdownMenu && subMenus) {
              // get the position and dimensions of the dropdown menu
              var dropdownOffset = subMenus.getBoundingClientRect();
              var dropdownWidth = subMenus.offsetWidth;
              var dropdownHeight = subMenus.offsetHeight;

              // get the dimensions of the screen
              var screenWidth = window.innerWidth;
              var screenHeight = window.innerHeight;

              // calculate the maximum x and y coordinates of the dropdown menu
              var maxDropdownX = dropdownOffset.left + dropdownWidth;
              var maxDropdownY = dropdownOffset.top + dropdownHeight;

              // check if the dropdown menu goes outside the screen
              var isDropdownOffScreen =
                maxDropdownX > screenWidth || maxDropdownY > screenHeight;
              if (isDropdownOffScreen) {
                if (isLeftFull) {
                  subMenus.classList.remove(
                    "group-data-[layout=horizontal]:ltr:md:left-full"
                  );
                  subMenus.classList.add(
                    "group-data-[layout=horizontal]:ltr:md:right-full"
                  );
                }
              } else if (isRightFull) {
                subMenus.classList.add(
                  "group-data-[layout=horizontal]:rtl:md:left-full"
                );
                subMenus.classList.remove(
                  "group-data-[layout=horizontal]:rtl:md:right-full"
                );
              }
            }
          }
        }
      }, 10);
    });
  });
}
</script>
<template>
  <div
    class="app-menu w-vertical-menu bg-vertical-menu ltr:border-r rtl:border-l border-vertical-menu-border fixed bottom-0 top-0 z-drawer md:z-[1003] transition-all duration-75 ease-linear group-data-[sidebar-size=md]:w-vertical-menu-md group-data-[sidebar-size=sm]:w-vertical-menu-sm group-data-[sidebar-size=sm]:pt-header group-data-[sidebar=dark]:bg-vertical-menu-dark group-data-[sidebar=dark]:border-vertical-menu-dark group-data-[sidebar=brand]:bg-vertical-menu-brand group-data-[sidebar=brand]:border-vertical-menu-brand group-data-[sidebar=modern]:bg-gradient-to-tr group-data-[sidebar=modern]:to-vertical-menu-to-modern group-data-[sidebar=modern]:from-vertical-menu-form-modern group-data-[layout=horizontal]:w-full group-data-[layout=horizontal]:bottom-auto group-data-[layout=horizontal]:top-header hidden md:block print:hidden group-data-[sidebar-size=sm]:absolute group-data-[sidebar=modern]:border-vertical-menu-border-modern group-data-[layout=horizontal]:dark:bg-zink-700 group-data-[layout=horizontal]:border-t group-data-[layout=horizontal]:dark:border-zink-500 group-data-[layout=horizontal]:border-r-0 group-data-[sidebar=dark]:dark:bg-zink-700 group-data-[sidebar=dark]:dark:border-zink-600 group-data-[layout=horizontal]:group-data-[navbar=scroll]:absolute group-data-[layout=horizontal]:group-data-[navbar=bordered]:top-[calc(theme('spacing.header')_+_theme('spacing.4'))] group-data-[layout=horizontal]:group-data-[navbar=bordered]:inset-x-4 group-data-[layout=horizontal]:group-data-[navbar=hidden]:top-0 group-data-[layout=horizontal]:group-data-[navbar=hidden]:h-16 group-data-[layout=horizontal]:group-data-[navbar=bordered]:w-[calc(100%_-_2rem)] group-data-[layout=horizontal]:group-data-[navbar=bordered]:[&.sticky]:top-header group-data-[layout=horizontal]:group-data-[navbar=bordered]:rounded-b-md group-data-[layout=horizontal]:shadow-md group-data-[layout=horizontal]:shadow-slate-500/10 group-data-[layout=horizontal]:dark:shadow-zink-500/10 [layout=horizontal]:ltr:pl-0 [layout=horizontal]:rtl:pr-0 md:group-data-[layout=horizontal]:z-10"
  >
    <div
      class="flex items-center justify-center px-5 text-center h-header group-data-[layout=horizontal]:hidden group-data-[sidebar-size=sm]:fixed group-data-[sidebar-size=sm]:top-0 group-data-[sidebar-size=sm]:bg-vertical-menu group-data-[sidebar-size=sm]:group-data-[sidebar=dark]:bg-vertical-menu-dark group-data-[sidebar-size=sm]:group-data-[sidebar=brand]:bg-vertical-menu-brand group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:bg-gradient-to-br group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:to-vertical-menu-to-modern group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:from-vertical-menu-form-modern group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:bg-vertical-menu-modern group-data-[sidebar-size=sm]:z-10 group-data-[sidebar-size=sm]:w-[calc(theme('spacing.vertical-menu-sm')_-_1px)] group-data-[sidebar-size=sm]:group-data-[sidebar=dark]:dark:bg-zink-700"
    >
      <div
        @click="onLogoClick"
        class="group-data-[sidebar=dark]:hidden group-data-[sidebar=brand]:hidden group-data-[sidebar=modern]:hidden cursor-pointer"
      >
        <span class="hidden group-data-[sidebar-size=sm]:block">
          <img :src="logoSm" alt="" class="h-6 mx-auto" />
        </span>
        <span class="group-data-[sidebar-size=sm]:hidden">
          <img :src="logoDark" alt="" class="h-6 mx-auto" />
        </span>
      </div>
      <div
        @click="onLogoClick"
        class="hidden group-data-[sidebar=dark]:block group-data-[sidebar=brand]:block group-data-[sidebar=modern]:block cursor-pointer"
      >
        <span class="hidden group-data-[sidebar-size=sm]:block">
          <img :src="logoSm" alt="" class="h-6 mx-auto" />
        </span>
        <span class="group-data-[sidebar-size=sm]:hidden">
          <img :src="logoLight" alt="" class="h-6 mx-auto" />
        </span>
      </div>
      <button type="button" class="hidden p-0 float-end" id="vertical-hover">
        <i class="ri-record-circle-line"></i>
      </button>
    </div>
    <component
      :is="sideBarComponent"
      id="menu-scrollbar"
      class="group-data-[sidebar-size=md]:max-h-[calc(100vh_-_theme('spacing.header')_*_1.2)] group-data-[sidebar-size=lg]:max-h-[calc(100vh_-_theme('spacing.header')_*_1.2)] group-data-[layout=horizontal]:h-56 group-data-[layout=horizontal]:md:h-auto group-data-[layout=horizontal]:overflow-auto group-data-[layout=horizontal]:md:overflow-visible group-data-[layout=horizontal]:max-w-screen-2xl group-data-[layout=horizontal]:mx-auto"
    >
      <div>
        <ul
          class="group-data-[layout=horizontal]:flex group-data-[layout=horizontal]:flex-col group-data-[layout=horizontal]:md:flex-row"
          id="navbar-nav"
        >
          <template v-for="menuItem in menuItemData" :key="menuItem.title">
            <li
              v-if="menuItem.isHeader"
              class="px-4 py-1 text-vertical-menu-item group-data-[sidebar=dark]:text-vertical-menu-item-dark group-data-[sidebar=brand]:text-vertical-menu-item-brand group-data-[sidebar=modern]:text-vertical-menu-item-modern uppercase font-medium text-[11px] cursor-default tracking-wider group-data-[sidebar-size=sm]:hidden group-data-[layout=horizontal]:hidden inline-block group-data-[sidebar-size=md]:block group-data-[sidebar-size=md]:underline group-data-[sidebar-size=md]:text-center group-data-[sidebar=dark]:dark:text-zink-200"
            >
              <span :data-key="`t-${menuItem.title}`">
                {{ $t(`t-${menuItem.title}`) }}
              </span>
            </li>
            <template v-else-if="menuItem.subMenu">
              <SubMenu
                :menuItem="menuItem"
                @toggleActivation="toggleActivation"
              />
            </template>
            <li
              v-else-if="!menuItem.subMenu && menuItem.path"
              class="relative group-data-[layout=horizontal]:shrink-0 group/sm"
            >
              <router-link
                class="relative flex items-center ltr:pl-3 rtl:pr-3 ltr:pr-5 rtl:pl-5 mx-3 my-1 group/menu-link text-vertical-menu-item-font-size font-normal transition-all duration-75 ease-linear rounded-md py-2.5 text-vertical-menu-item hover:text-vertical-menu-item-hover hover:bg-vertical-menu-item-bg-hover [&.active]:text-vertical-menu-item-active [&.active]:bg-vertical-menu-item-bg-active group-data-[sidebar=dark]:text-vertical-menu-item-dark group-data-[sidebar=dark]:hover:text-vertical-menu-item-hover-dark group-data-[sidebar=dark]:dark:hover:text-custom-500 group-data-[layout=horizontal]:dark:hover:text-custom-500 group-data-[sidebar=dark]:hover:bg-vertical-menu-item-bg-hover-dark group-data-[sidebar=dark]:dark:hover:bg-zink-600 group-data-[sidebar=dark]:[&.active]:text-vertical-menu-item-active-dark group-data-[sidebar=dark]:[&.active]:bg-vertical-menu-item-bg-active-dark group-data-[sidebar=brand]:text-vertical-menu-item-brand group-data-[sidebar=brand]:hover:text-vertical-menu-item-hover-brand group-data-[sidebar=brand]:hover:bg-vertical-menu-item-bg-hover-brand group-data-[sidebar=brand]:[&.active]:bg-vertical-menu-item-bg-active-brand group-data-[sidebar=brand]:[&.active]:text-vertical-menu-item-active-brand group-data-[sidebar=modern]:text-vertical-menu-item-modern group-data-[sidebar=modern]:hover:bg-vertical-menu-item-bg-hover-modern group-data-[sidebar=modern]:hover:text-vertical-menu-item-hover-modern group-data-[sidebar=modern]:[&.active]:bg-vertical-menu-item-bg-active-modern group-data-[sidebar=modern]:[&.active]:text-vertical-menu-item-active-modern group-data-[sidebar-size=md]:block group-data-[sidebar-size=md]:text-center group-data-[sidebar-size=sm]:group-hover/sm:w-[calc(theme('spacing.vertical-menu-sm')_*_3.63)] group-data-[sidebar-size=sm]:group-hover/sm:bg-vertical-menu group-data-[sidebar-size=sm]:group-data-[sidebar=dark]:group-hover/sm:bg-vertical-menu-dark group-data-[sidebar-size=sm]:group-data-[sidebar=modern]:group-hover/sm:bg-vertical-menu-border-modern group-data-[sidebar-size=sm]:group-data-[sidebar=brand]:group-hover/sm:bg-vertical-menu-brand group-data-[sidebar-size=sm]:my-0 group-data-[layout=horizontal]:m-0 group-data-[layout=horizontal]:ltr:pr-8 group-data-[layout=horizontal]:rtl:pl-8 group-data-[layout=horizontal]:hover:bg-transparent group-data-[layout=horizontal]:[&.active]:bg-transparent [&.dropdown-button]:before:absolute [&.dropdown-button]:[&.show]:before:content-['\ea4e'] [&.dropdown-button]:before:content-['\ea6e'] [&.dropdown-button]:before:font-remix ltr:[&.dropdown-button]:before:right-2 rtl:[&.dropdown-button]:before:left-2 [&.dropdown-button]:before:text-16 group-data-[sidebar-size=sm]:[&.dropdown-button]:before:hidden group-data-[sidebar-size=md]:[&.dropdown-button]:before:hidden group-data-[sidebar=dark]:dark:text-zink-200 group-data-[layout=horizontal]:dark:text-zink-200 group-data-[sidebar=dark]:[&.active]:dark:bg-zink-600 group-data-[layout=horizontal]:dark:[&.active]:text-custom-500 rtl:[&.dropdown-button]:before:rotate-180 group-data-[layout=horizontal]:[&.dropdown-button]:before:rotate-90 group-data-[layout=horizontal]:[&.dropdown-button]:[&.show]:before:rotate-0 rtl:[&.dropdown-button]:[&.show]:before:rotate-0"
                :class="{
                  active: path === menuItem.path
                }"
                :to="menuItem.path"
                @click="toggleActivation"
              >
                <span
                  class="min-w-[1.75rem] group-data-[sidebar-size=sm]:h-[1.75rem] inline-block text-start text-[16px] group-data-[sidebar-size=md]:block"
                >
                  <component
                    :is="menuItem.icon"
                    class="h-4 group-data-[sidebar-size=sm]:h-5 group-data-[sidebar-size=sm]:w-5 transition group-hover/menu-link:animate-icons fill-slate-100 group-hover/menu-link:fill-blue-200 group-data-[sidebar=dark]:fill-vertical-menu-item-bg-active-dark group-data-[sidebar=dark]:dark:fill-zink-600 group-data-[layout=horizontal]:dark:fill-zink-600 group-data-[sidebar=brand]:fill-vertical-menu-item-bg-active-brand group-data-[sidebar=modern]:fill-vertical-menu-item-bg-active-modern group-data-[sidebar=dark]:group-hover/menu-link:fill-vertical-menu-item-bg-active-dark group-data-[sidebar=dark]:group-hover/menu-link:dark:fill-custom-500/20 group-data-[layout=horizontal]:dark:group-hover/menu-link:fill-custom-500/20 group-data-[sidebar=brand]:group-hover/menu-link:fill-vertical-menu-item-bg-active-brand group-data-[sidebar=modern]:group-hover/menu-link:fill-vertical-menu-item-bg-active-modern group-data-[sidebar-size=md]:block group-data-[sidebar-size=md]:mx-auto group-data-[sidebar-size=md]:mb-2"
                  />
                </span>
                <span
                  class="group-data-[sidebar-size=sm]:ltr:pl-10 group-data-[sidebar-size=sm]:rtl:pr-10 align-middle group-data-[sidebar-size=sm]:group-hover/sm:block group-data-[sidebar-size=sm]:hidden"
                  :data-key="`t-${menuItem.title}`"
                >
                  {{ $t(`t-${menuItem.title}`) }}
                </span>
              </router-link>
            </li>
          </template>
        </ul>
      </div>
    </component>
  </div>
</template>
