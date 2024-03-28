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
  /*
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
  */
];
