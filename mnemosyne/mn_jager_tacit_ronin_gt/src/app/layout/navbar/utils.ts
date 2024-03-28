import {
  usFlag,
  esFlag,
  deFlag,
  frFlag,
  jpFlag,
  chinaFlag,
  itFlag,
  ruFlag,
  aeFlag
} from "@/assets/images/flags/utils";
import { ShoppingBag } from "lucide-vue-next";
import { img01, img03, img09 } from "@/assets/images/product/utils";
import { CartItemType } from "@/app/layout/navbar/type";
import { avatar5, avatar3, avatar7 } from "@/assets/images/users/utils";

export const languagesData: {
  title: string;
  value: string;
  src: string;
  lang: string;
}[] = [
  { title: "English", value: "English", src: usFlag, lang: "en" },
  { title: "Spanish", value: "Spanish", src: esFlag, lang: "sp" },
  { title: "German", value: "German", src: deFlag, lang: "gr" },
  { title: "French", value: "French", src: frFlag, lang: "fr" },
  { title: "Japanese", value: "Japanese", src: jpFlag, lang: "jp" },
  { title: "Chinese", value: "Chinese", src: chinaFlag, lang: "ch" },
  { title: "Italian", value: "Italian", src: itFlag, lang: "it" },
  { title: "Russian", value: "Russian", src: ruFlag, lang: "ru" },
  { title: "Arabic", value: "Arabic", src: aeFlag, lang: "ar" }
];

export const cartItems: CartItemType[] = [
  {
    src: img01,
    title: "Cotton collar t-shirts for men",
    type: "Fashion",
    count: 2,
    price: 155.32
  },
  {
    src: img03,
    title: "Like style travel black handbag",
    type: "Luggage",
    count: 1,
    price: 349.95
  },
  {
    src: img09,
    title: "Blive Printed Men Round Neck",
    type: "Fashion",
    count: 4,
    price: 546.74
  }
];

export const notificationData = [
  {
    src: avatar3,
    title: "<b>@willie_passem</b> followed you",
    fullTime: "Wednesday 03:42 PM",
    time: "4 sec",
    type: "followers"
  },
  {
    src: avatar5,
    title: "<b>@caroline_jessica</b> commented on your post",
    message: `Amazing! Fast, to the point, professional and really amazing
    to work with them!!!`,
    fullTime: "Wednesday 03:42 PM",
    time: "15 min",
    type: "mentions"
  },
  {
    icon: ShoppingBag,
    title: `Successfully purchased a business plan for
    <span class="text-red-500">$199.99</span>`,
    fullTime: "Monday 11:26 AM",
    time: "Yesterday",
    type: "invites"
  },
  {
    src: avatar7,
    title: `<b>@scott</b> liked your post`,
    fullTime: "Thursday 06:59 AM",
    time: "1 Week",
    isLike: true,
    type: "mentions"
  }
];
