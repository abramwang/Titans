import { Component } from "vue";

export interface SubMenuType {
  title: string;
  path?: string;
  id?: string;
  isActive?: boolean;
  subMenu?: SubMenuType[];
}

export interface MenuItemType {
  id?: string;
  isHeader?: boolean;
  icon?: Component;
  title?: string;
  path?: string;
  isActive?: boolean;
  subMenu?: SubMenuType[];
}
