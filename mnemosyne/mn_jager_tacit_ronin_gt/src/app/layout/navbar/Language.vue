<script lang="ts" setup>
import { usFlag } from "@/assets/images/flags/utils";
import { languagesData } from "@/app/layout/navbar/utils";
import NavBtn from "@/app/layout/navbar/Button.vue";
import { useI18n } from "vue-i18n";
import { ref } from "vue";
const i18n = useI18n();

defineProps({
  showName: {
    type: Boolean,
    default: false
  }
});
const langTitle = ref("English");

const setLanguage = (entry: { [key: string]: string }) => {
  const { src, lang, title } = entry;
  const element = document.getElementById("header-lang-img");
  if (element) {
    element.setAttribute("src", src);
  }
  i18n.locale.value = lang;
  langTitle.value = title;
};
</script>
<template>
  <TList :items="languagesData" placement="bottom-start">
    <template #title>
      <button
        v-if="showName"
        type="button"
        class="inline-flex items-center gap-3 transition-all duration-200 ease-linear dropdown-toggle btn border-slate-200 dark:border-zink-400/60 group/items focus:border-custom-500 dark:focus:border-custom-500"
      >
        <img
          :src="usFlag"
          alt="language-flag"
          id="header-lang-img"
          class="object-cover h-5 rounded-full"
        />
        <h6
          class="text-base font-medium transition-all duration-200 ease-linear text-slate-600 group-hover/items:text-custom-500 dark:text-zink-200 dark:group-hover/items:text-custom-500"
        >
          {{ langTitle }}
        </h6>
      </button>
      <NavBtn class="dropdown" v-else>
        <img
          :src="usFlag"
          alt="language-flag"
          id="header-lang-img"
          class="h-5 rounded-sm"
        />
      </NavBtn>
    </template>
    <template #default="{ data }">
      <div
        class="flex items-center gap-3 group/items language"
        :data-lang="data.lang"
        :title="data.title"
        @click.prevent="setLanguage(data)"
      >
        <img :src="data.src" alt="" class="object-cover h-4 rounded-full" />
        <h6
          class="transition-all duration-200 ease-linear font-15medium text- text-slate-600 dark:text-zink-200 group-hover/items:text-custom-500"
        >
          {{ data.title }}
        </h6>
      </div>
    </template>
  </TList>
</template>
