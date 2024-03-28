import {
  MonitorDot,
  PictureInPicture2,
  MessagesSquare,
  Mail,
  CalendarDays,
  ShoppingBag,
  CircuitBoard,
  ScrollText,
  RadioTower,
  FileText,
  User2,
  Award,
  Codesandbox,
  LifeBuoy,
  PackagePlus,
  LocateFixed,
  Table,
  PieChart,
  Trophy,
  Map,
  Share2
} from "lucide-vue-next";
import { MenuItemType } from "@/app/layout/types";

export const menuItems: MenuItemType[] = [
  { isHeader: true, title: "menu" },
  {
    icon: MonitorDot,
    title: "dashboards",

    subMenu: [
      { title: "ecommerce", path: "/" },
    ]
  },
  { isHeader: true, title: "pages" },
  {
    title: "authentication",
    icon: Award,
    subMenu: [
      {
        title: "login",
        subMenu: [
          { path: "/login/basic", title: "basic" },
          { path: "/login/cover", title: "cover" },
          { path: "/login/boxed", title: "boxed" },
          { path: "/login/modern", title: "modern" }
        ]
      },
      {
        title: "register",
        subMenu: [
          { path: "/register/basic", title: "basic" },
          { path: "/register/cover", title: "cover" },
          { path: "/register/boxed", title: "boxed" },
          { path: "/register/modern", title: "modern" }
        ]
      },
      {
        title: "verify-email",
        subMenu: [
          { path: "/verify-email/basic", title: "basic" },
          { path: "/verify-email/cover", title: "cover" },
          { path: "/verify-email/modern", title: "modern" }
        ]
      },
      {
        title: "two-steps",
        subMenu: [
          { path: "/two-steps/basic", title: "basic" },
          { path: "/two-steps/cover", title: "cover" },
          { path: "/two-steps/boxed", title: "boxed" },
          { path: "/two-steps/modern", title: "modern" }
        ]
      },
      {
        title: "logout",
        subMenu: [
          { path: "/logout/basic", title: "basic" },
          { path: "/logout/cover", title: "cover" },
          { path: "/logout/boxed", title: "boxed" },
          { path: "/logout/modern", title: "modern" }
        ]
      },
      {
        title: "reset-password",
        subMenu: [
          { path: "/reset-password/basic", title: "basic" },
          { path: "/reset-password/cover", title: "cover" },
          { path: "/reset-password/boxed", title: "boxed" },
          { path: "/reset-password/modern", title: "modern" }
        ]
      },
      {
        title: "create-password",
        subMenu: [
          { path: "/create-password/basic", title: "basic" },
          { path: "/create-password/cover", title: "cover" },
          { path: "/create-password/boxed", title: "boxed" },
          { path: "/create-password/modern", title: "modern" }
        ]
      }
    ]
  },
  {
    title: "pages",
    icon: Codesandbox,
    subMenu: [
      { path: "/pages/coming-soon", title: "coming-soon" },
      { path: "/pages/maintenance", title: "maintenance" }
    ]
  },
  { isHeader: true, title: "components" },
  {
    title: "multi-level",
    icon: Share2,
    subMenu: [
      { path: "#", title: "level-1.1" },
      {
        title: "level-1.2",
        subMenu: [
          { title: "level-2.1", path: "#" },
          { title: "level-2.2", path: "#" }
        ]
      }
    ]
  }
];
