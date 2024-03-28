<script lang="ts" setup>
import { computed, type PropType } from "vue";
import { X } from "lucide-vue-next";
const props = defineProps({
  color: {
    type: String,
    default: ""
  },
  variant: {
    type: String as PropType<
      | "outlined"
      | "bordered"
      | "soft-outlined"
      | "pill"
      | "pill-outlined"
      | "fill"
      | "link"
      | "plain"
    >,
    default: ""
  },
  dismissible: {
    type: Boolean,
    default: false
  },
  icon: {
    type: Boolean,
    default: false
  },
  rounded: {
    type: Boolean,
    default: false
  }
});

const getClass = computed(() => {
  switch (props.color) {
    case "yellow":
      if (props.variant === "outlined") {
        return "bg-white border-yellow-400 text-yellow-500 dark:bg-zink-700 dark:border-yellow-700";
      } else if (props.variant === "bordered") {
        return "bg-yellow-100 border-yellow-200 text-yellow-500 dark:bg-yellow-500/20 dark:border-yellow-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-yellow-100 text-yellow-500 dark:bg-zink-700 dark:border-yellow-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-yellow-100 border-transparent text-yellow-500 dark:bg-yellow-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full border-yellow-400 text-yellow-500 dark:bg-zink-700 dark:border-yellow-700";
      } else if (props.variant === "fill") {
        return "bg-yellow-500 border-yellow-500 text-yellow-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-yellow-100 border-transparent text-yellow-500 hover:bg-yellow-200 dark:bg-yellow-400/20 dark:hover:bg-yellow-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-yellow-500 bg-white border-transparent dark:bg-yellow-500/20 dark:border-transparent";
      }
      return "bg-yellow-100 text-yellow-500 dark:bg-yellow-500/20 border-transparent";

    case "green":
      if (props.variant === "outlined") {
        return "bg-white border-green-400 text-green-500 dark:bg-zink-700 dark:border-green-700";
      } else if (props.variant === "bordered") {
        return "bg-green-100 border-green-200 text-green-500 dark:bg-green-500/20 dark:border-green-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-green-100 text-green-500 dark:bg-zink-700 dark:border-green-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-green-100 border-transparent text-green-500 dark:bg-green-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full border-green-400 text-green-500 dark:bg-zink-700 dark:border-green-700";
      } else if (props.variant === "fill") {
        return "bg-green-500 border-green-500 text-green-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-green-100 border-transparent text-green-500 hover:bg-green-200 dark:bg-green-400/20 dark:hover:bg-green-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-green-500 bg-white border-transparent dark:bg-green-500/20 dark:border-transparent";
      }
      return "bg-green-100 text-green-500 dark:bg-green-500/20 border-transparent";

    case "red":
      if (props.variant === "outlined") {
        return "bg-white border-red-400 text-red-500 dark:bg-zink-700 dark:border-red-700";
      } else if (props.variant === "bordered") {
        return "bg-red-100 border-red-200 text-red-500 dark:bg-red-500/20 dark:border-red-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-red-100 text-red-500 dark:bg-zink-700 dark:border-red-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-red-100 border-transparent text-red-500 dark:bg-red-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full border-red-400 text-red-500 dark:bg-zink-700 dark:border-red-700";
      } else if (props.variant === "fill") {
        return "bg-red-500 border-red-500 text-red-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-red-100 border-transparent text-red-500 hover:bg-red-200 dark:bg-red-400/20 dark:hover:bg-red-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-red-500 bg-white border-transparent dark:bg-red-500/20 dark:border-transparent";
      }
      return "bg-red-100 text-red-500 dark:bg-red-500/20 border-transparent";

    case "orange":
      if (props.variant === "outlined") {
        return "bg-white border-orange-400 text-orange-500 dark:bg-zink-700 dark:border-orange-700";
      } else if (props.variant === "bordered") {
        return "bg-orange-100 border-orange-200 text-orange-500 dark:bg-orange-500/20 dark:border-orange-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-orange-100 text-orange-500 dark:bg-zink-700 dark:border-orange-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-orange-100 border-transparent text-orange-500 dark:bg-orange-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full border-orange-400 text-orange-500 dark:bg-zink-700 dark:border-orange-700";
      } else if (props.variant === "fill") {
        return "bg-orange-500 border-orange-500 text-orange-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-orange-100 border-transparent text-orange-500 hover:bg-orange-200 dark:bg-orange-400/20 dark:hover:bg-orange-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-orange-500 bg-white border-transparent dark:bg-orange-500/20 dark:border-transparent";
      }
      return "bg-orange-100 text-orange-500 dark:bg-orange-500/20 border-transparent";

    case "purple":
      if (props.variant === "outlined") {
        return "bg-white border-purple-400 text-purple-500 dark:bg-zink-700 dark:border-purple-700";
      } else if (props.variant === "bordered") {
        return "bg-purple-100 border-purple-200 text-purple-500 dark:bg-purple-500/20 dark:border-purple-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-purple-100 text-purple-500 dark:bg-zink-700 dark:border-purple-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-purple-100 border-transparent text-purple-500 dark:bg-purple-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full border-purple-400 text-purple-500 dark:bg-zink-700 dark:border-purple-700";
      } else if (props.variant === "fill") {
        return "bg-purple-500 border-purple-500 text-purple-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-purple-100 border-transparent text-purple-500 hover:bg-purple-200 dark:bg-purple-400/20 dark:hover:bg-purple-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-purple-500 bg-white border-transparent dark:bg-purple-500/20 dark:border-transparent";
      }
      return "bg-purple-100 text-purple-500 dark:bg-purple-500/20 border-transparent";

    case "sky":
      if (props.variant === "outlined") {
        return "bg-white border-sky-400 text-sky-500 dark:bg-zink-700 dark:border-sky-700";
      } else if (props.variant === "bordered") {
        return "bg-sky-100 border-sky-200 text-sky-500 dark:bg-sky-500/20 dark:border-sky-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-sky-100 text-sky-500 dark:bg-zink-700 dark:border-sky-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-sky-100 border-transparent text-sky-500 dark:bg-sky-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full border-sky-400 text-sky-500 dark:bg-zink-700 dark:border-sky-700";
      } else if (props.variant === "fill") {
        return "bg-sky-500 border-sky-500 text-sky-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-sky-100 border-transparent text-sky-500 hover:bg-sky-200 dark:bg-sky-400/20 dark:hover:bg-sky-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-sky-500 bg-white border-transparent dark:bg-sky-500/20 dark:border-transparent";
      }
      return "bg-sky-100 text-sky-500 dark:bg-sky-500/20 border-transparent";

    case "slate":
      if (props.variant === "outlined") {
        return "bg-white border-slate-400 text-slate-500 dark:bg-zink-700 dark:border-slate-700";
      } else if (props.variant === "bordered") {
        return "bg-slate-100 border-slate-200 text-slate-500 dark:bg-slate-500/20 dark:border-slate-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-slate-100 text-slate-500 dark:bg-zink-700 dark:border-slate-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-slate-100 border-transparent text-slate-500 dark:bg-slate-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full border-slate-400 text-slate-500 dark:bg-zink-700 dark:border-slate-700";
      } else if (props.variant === "fill") {
        return "bg-slate-500 border-slate-500 text-slate-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-slate-100 border-transparent text-slate-500 hover:bg-slate-200 dark:bg-slate-400/20 dark:hover:bg-slate-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-slate-500 bg-white border-transparent dark:bg-slate-500/20 dark:border-transparent";
      }
      return "bg-slate-100 text-slate-500 dark:bg-slate-500/20 border-transparent";

    default:
      if (props.variant === "outlined") {
        return "bg-white border-custom-400 text-custom-500 dark:bg-zink-700 dark:border-custom-700";
      } else if (props.variant === "bordered") {
        return "bg-custom-100 border-custom-200 text-custom-500 dark:bg-custom-500/20 dark:border-custom-500/20";
      } else if (props.variant === "soft-outlined") {
        return "bg-white border-custom-100 text-custom-500 dark:bg-zink-700 dark:border-custom-900";
      } else if (props.variant === "pill") {
        return "rounded-full bg-custom-100 border-transparent text-custom-500 dark:bg-custom-500/20 dark:border-transparent";
      } else if (props.variant === "pill-outlined") {
        return "rounded-full  border-custom-400 text-custom-500 dark:bg-zink-700 dark:border-custom-700";
      } else if (props.variant === "fill") {
        return "bg-custom-500 border-custom-500 text-custom-50";
      } else if (props.variant === "link") {
        return "cursor-pointer transition-all duration-200 ease-linear bg-custom-100 border-transparent text-custom-500 hover:bg-custom-200 dark:bg-custom-400/20 dark:hover:bg-custom-400/30 dark:border-transparent";
      } else if (props.variant === "plain") {
        return "text-custom-500 bg-white border-transparent dark:bg-custom-500/20 dark:border-transparent";
      }
      return "bg-custom-100 text-custom-500 dark:bg-custom-500/20 border-transparent";
  }
});

const onClick = (event: any) => {
  const element = event.target.closest("[label]");
  if (element) {
    element.classList.add("hidden");
  }
};
</script>
<template>
  <span
    :class="` font-medium ${getClass}  inline-flex text-xs border ${
      icon ? ' size-8' : 'px-2.5 py-0.5'
    } first-letter:
    ${rounded ? 'rounded-full' : ''}
    `"
    class="rounded flex items-center justify-center"
    label
  >
    <slot />
    <X
      v-if="dismissible"
      class="size-3 ltr:ml-1 rtl:mr-1 cursor-pointer"
      @click="onClick"
    />
  </span>
</template>
