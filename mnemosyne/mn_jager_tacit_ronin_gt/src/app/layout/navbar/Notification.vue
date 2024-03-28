<script lang="ts" setup>
import NavBtn from "@/app/layout/navbar/Button.vue";
import { BellRing, Clock, MoveRight, Heart } from "lucide-vue-next";
import { ref } from "vue";
import { notificationData } from "@/app/layout/navbar/utils.ts";
import { computed } from "vue";

const tab = ref("viewAll");
const getParsedData = computed(() => {
  if (tab.value === "viewAll") {
    return notificationData;
  }
  return notificationData.filter((item) => item.type === tab.value);
});
</script>
<template>
  <TMenu>
    <NavBtn class="dropdown">
      <BellRing
        class="inline-block size-5 stroke-1 fill-slate-100 group-data-[topbar=dark]:fill-topbar-item-bg-hover-dark group-data-[topbar=brand]:fill-topbar-item-bg-hover-brand"
      />
      <span class="absolute top-0 right-0 flex w-1.5 h-1.5">
        <span
          class="absolute inline-flex w-full h-full rounded-full opacity-75 animate-ping bg-sky-400"
        />
        <span
          class="relative inline-flex w-1.5 h-1.5 rounded-full bg-sky-500"
        />
      </span>
    </NavBtn>
    <template #content>
      <div class="min-w-[20rem] lg:min-w-[26rem]">
        <div class="p-4">
          <h6 class="mb-4 text-16">
            Notifications
            <span
              class="inline-flex items-center justify-center size-5 ml-1 text-[11px] font-medium border rounded-full text-white bg-orange-500 border-orange-500"
            >
              15
            </span>
          </h6>
          <TTabs
            v-model="tab"
            class="flex flex-wrap w-full p-1 mb-2 text-sm font-medium text-center rounded-md filter-btns text-slate-500 bg-slate-100 nav-tabs dark:bg-zink-500 dark:text-zink-200"
          >
            <TTab
              value="viewAll"
              grow
              tabClass="inline-block nav-link px-1.5 w-full py-1 text-xs transition-all duration-300 ease-linear rounded-md text-slate-500 border border-transparent group-[.active]:bg-white group-[.active]:text-custom-500 hover:text-custom-500 active:text-custom-500 dark:text-zink-200 dark:hover:text-custom-500 dark:group-[.active]:bg-zink-600 -mb-[1px]"
            >
              View All
            </TTab>
            <TTab
              tabClass="inline-block nav-link px-1.5 w-full py-1 text-xs transition-all duration-300 ease-linear rounded-md text-slate-500 border border-transparent group-[.active]:bg-white group-[.active]:text-custom-500 hover:text-custom-500 active:text-custom-500 dark:text-zink-200 dark:hover:text-custom-500 dark:group-[.active]:bg-zink-600 -mb-[1px]"
              value="mentions"
              grow
            >
              Mentions
            </TTab>
            <TTab
              tabClass="inline-block nav-link px-1.5 w-full py-1 text-xs transition-all duration-300 ease-linear rounded-md text-slate-500 border border-transparent group-[.active]:bg-white group-[.active]:text-custom-500 hover:text-custom-500 active:text-custom-500 dark:text-zink-200 dark:hover:text-custom-500 dark:group-[.active]:bg-zink-600 -mb-[1px]"
              value="followers"
              grow
            >
              Followers
            </TTab>
            <TTab
              tabClass="inline-block nav-link px-1.5 w-full py-1 text-xs transition-all duration-300 ease-linear rounded-md text-slate-500 border border-transparent group-[.active]:bg-white group-[.active]:text-custom-500 hover:text-custom-500 active:text-custom-500 dark:text-zink-200 dark:hover:text-custom-500 dark:group-[.active]:bg-zink-600 -mb-[1px]"
              value="invites"
              grow
            >
              Invites
            </TTab>
          </TTabs>
        </div>
        <simplebar class="max-h-[350px]">
          <div class="flex flex-col gap-1">
            <a
              v-for="(item, index) in getParsedData"
              :key="'notification-' + index"
              href="#!"
              class="flex gap-3 p-4 product-item hover:bg-slate-50 dark:hover:bg-zink-500 follower"
            >
              <div class="relative shrink-0">
                <div
                  v-if="item.isLike"
                  class="absolute text-orange-500 -bottom-0.5 -right-0.5 text-16"
                >
                  <Heart fill="orange" class="size-4"></Heart>
                </div>
                <div
                  v-if="item.src"
                  class="size-10 rounded-md shrink-0 bg-slate-100"
                >
                  <img :src="item.src" alt="" class="rounded-md" />
                </div>
                <div
                  v-else
                  class="flex items-center justify-center size-10 bg-red-100 rounded-md shrink-0"
                >
                  <component
                    :is="item.icon"
                    class="size-5 text-red-500 fill-red-200"
                  />
                </div>
              </div>

              <div class="grow">
                <h6 class="mb-1 font-medium" v-html="item.title"></h6>
                <p class="mb-0 text-sm text-slate-500 dark:text-zink-300">
                  <Clock class="inline-block w-3.5 h-3.5 mr-1" />
                  <span class="align-middle">{{ item.fullTime }}</span>
                </p>
              </div>
              <div
                class="flex items-center self-start gap-2 text-xs text-slate-500 shrink-0 dark:text-zink-300"
              >
                <div class="w-1.5 h-1.5 bg-custom-500 rounded-full"></div>
                {{ item.time }}
              </div>
            </a>
          </div>
        </simplebar>
        <div
          class="flex items-center gap-2 p-4 border-t border-slate-200 dark:border-zink-500"
        >
          <div class="grow">
            <a href="#!">Manage Notification</a>
          </div>
          <div class="shrink-0">
            <button
              type="button"
              class="px-2 py-1.5 text-xs text-white transition-all duration-200 ease-linear btn bg-custom-500 border-custom-500 hover:text-white hover:bg-custom-600 hover:border-custom-600 focus:text-white focus:bg-custom-600 focus:border-custom-600 focus:ring focus:ring-custom-100 active:text-white active:bg-custom-600 active:border-custom-600 active:ring active:ring-custom-100"
            >
              View All Notification
              <MoveRight class="inline-block w-3.5 h-3.5 ml-1" />
            </button>
          </div>
        </div>
      </div>
    </template>
  </TMenu>
</template>
