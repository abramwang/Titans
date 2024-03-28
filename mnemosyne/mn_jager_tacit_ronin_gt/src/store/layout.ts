import { defineStore } from "pinia";
import { setAttribute } from "@/app/utils.ts";
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

export const useLayoutStore = defineStore("layout-store", {
  state: (): { [key: string]: string } => ({
    mode: SITE_MODE.LIGHT,
    layoutType: LAYOUTS.VERTICAL,
    layoutWidth: LAYOUT_WIDTH.FLUID,
    topBarColor: TOP_BAR.LIGHT,
    sideBarSize: SIDEBAR_SIZE.DEFAULT,
    sideBarColor: SIDEBAR_COLOR.LIGHT,
    dir: DIR.LTR,
    skinLayout: SKIN_LAYOUT.DEFAULT,
    navType: NAV_BAR_TYPE.STICKY
  }),
  actions: {
    changeSiteMode(mode: string) {
      this.mode = mode;
      this.topBarColor = mode;
      this.sideBarColor = mode;
      setAttribute("data-mode", this.mode);
      setAttribute("data-topbar", this.mode);
      setAttribute("data-sidebar", this.mode);
    },
    changeLayoutType(layoutType: string) {
      this.layoutType = layoutType;
      if (layoutType === LAYOUTS.HORIZONTAL) {
        this.changeSideBarSize(SIDEBAR_SIZE.DEFAULT);
        if (this.sideBarColor === SIDEBAR_COLOR.MODERN) {
          this.changeSideBarColor(this.topBarColor);
        }
      } else {
        this.changeSideBarColor(this.sideBarColor);
      }
      setAttribute("data-layout", layoutType);
    },
    changeTopBarColor(topBarColor: string) {
      this.topBarColor = topBarColor;
      setAttribute("data-topbar", topBarColor);
    },
    changeSkinLayout(skinLayout: string) {
      this.skinLayout = skinLayout;
      setAttribute("data-skin", skinLayout);
    },
    changeDir(dir: string) {
      this.dir = dir;
      setAttribute("dir", dir);
    },
    changeLayoutWidth(layoutWidth: string) {
      this.layoutWidth = layoutWidth;
      setAttribute("data-content", layoutWidth);
    },
    changeSideBarSize(sideBarSize: string) {
      this.sideBarSize = sideBarSize;
      setAttribute("data-sidebar-size", sideBarSize);
    },
    changeNavType(navType: string) {
      this.navType = navType;
      setAttribute("data-navbar", navType);
    },
    changeSideBarColor(sideBarColor: string) {
      this.sideBarColor = sideBarColor;
      setAttribute("data-sidebar", sideBarColor);
    }
  }
});
