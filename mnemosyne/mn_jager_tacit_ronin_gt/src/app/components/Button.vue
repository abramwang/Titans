<script lang="ts" setup>
import { computed, type PropType } from "vue";

const props = defineProps({
  color: {
    type: String,
    default: "custom"
  },
  variant: {
    type: String as PropType<
      | "soft"
      | "dashed"
      | "outlined"
      | "ghost"
      | "link"
      | "plain-fill"
      | "plain-soft"
    >,
    default: ""
  },
  classes: {
    type: String,
    default: ""
  },
  rounded: {
    type: Boolean,
    default: false
  },
  icon: {
    type: Boolean,
    default: false
  },
  border: {
    type: Boolean,
    default: true
  }
});

const getClass = computed(() => {
  switch (props.color) {
    case "yellow":
      if (props.variant === "soft") {
        return "text-yellow-500 btn bg-yellow-100 hover:text-white hover:bg-yellow-600 focus:text-white focus:bg-yellow-600 focus:ring focus:ring-yellow-100 active:text-white active:bg-yellow-600 active:ring active:ring-yellow-100 dark:bg-yellow-500/20 dark:text-yellow-500 dark:hover:bg-yellow-500 dark:hover:text-white dark:focus:bg-yellow-500 dark:focus:text-white dark:active:bg-yellow-500 dark:active:text-white dark:ring-yellow-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-yellow-500 btn border-yellow-500 hover:text-yellow-500 hover:bg-yellow-50 hover:border-yellow-600 focus:text-yellow-600 focus:bg-yellow-50 focus:border-yellow-600 active:text-yellow-600 active:bg-yellow-50 active:border-yellow-600 dark:bg-zink-700 dark:ring-yellow-400/20 dark:hover:bg-yellow-800/20 dark:focus:bg-yellow-800/20 dark:active:bg-yellow-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-yellow-500 btn border-yellow-500 hover:text-white hover:bg-yellow-600 hover:border-yellow-600 focus:text-white focus:bg-yellow-600 focus:border-yellow-600 focus:ring focus:ring-yellow-100 active:text-white active:bg-yellow-600 active:border-yellow-600 active:ring active:ring-yellow-100 dark:bg-zink-700 dark:hover:bg-yellow-500 dark:ring-yellow-400/20 dark:focus:bg-yellow-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-yellow-500 btn hover:text-yellow-500 hover:bg-yellow-100 focus:text-yellow-500 focus:bg-yellow-100 active:text-yellow-500 active:bg-yellow-100 dark:bg-zink-700 dark:hover:bg-yellow-500/10 dark:focus:bg-yellow-500/10 dark:active:bg-yellow-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-yellow-500 btn hover:text-yellow-700 focus:text-yellow-700 active:text-yellow-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-yellow-300 hover:text-yellow-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-yellow-400 hover:text-yellow-600";
      }
      return "text-white bg-yellow-500 border-yellow-500 hover:text-white hover:bg-yellow-600 hover:border-yellow-600 focus:text-white focus:bg-yellow-600 focus:border-yellow-600 focus:ring focus:ring-yellow-100 active:text-white active:bg-yellow-600 active:border-yellow-600 active:ring active:ring-yellow-100 dark:ring-yellow-400/20";
    case "green":
      if (props.variant === "soft") {
        return "text-green-500 btn bg-green-100 hover:text-white hover:bg-green-600 focus:text-white focus:bg-green-600 focus:ring focus:ring-green-100 active:text-white active:bg-green-600 active:ring active:ring-green-100 dark:bg-green-500/20 dark:text-green-500 dark:hover:bg-green-500 dark:hover:text-white dark:focus:bg-green-500 dark:focus:text-white dark:active:bg-green-500 dark:active:text-white dark:ring-green-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-green-500 btn border-green-500 hover:text-green-500 hover:bg-green-50 hover:border-green-600 focus:text-green-600 focus:bg-green-50 focus:border-green-600 active:text-green-600 active:bg-green-50 active:border-green-600 dark:bg-zink-700 dark:ring-green-400/20 dark:hover:bg-green-800/20 dark:focus:bg-green-800/20 dark:active:bg-green-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-green-500 btn border-green-500 hover:text-white hover:bg-green-600 hover:border-green-600 focus:text-white focus:bg-green-600 focus:border-green-600 focus:ring focus:ring-green-100 active:text-white active:bg-green-600 active:border-green-600 active:ring active:ring-green-100 dark:bg-zink-700 dark:hover:bg-green-500 dark:ring-green-400/20 dark:focus:bg-green-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-green-500 btn hover:text-green-500 hover:bg-green-100 focus:text-green-500 focus:bg-green-100 active:text-green-500 active:bg-green-100 dark:bg-zink-700 dark:hover:bg-green-500/10 dark:focus:bg-green-500/10 dark:active:bg-green-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-green-500 btn hover:text-green-700 focus:text-green-700 active:text-green-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-green-300 hover:text-green-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-green-400 hover:text-green-600";
      }
      return "text-white bg-green-500 border-green-500 hover:text-white hover:bg-green-600 hover:border-green-600 focus:text-white focus:bg-green-600 focus:border-green-600 focus:ring focus:ring-green-100 active:text-white active:bg-green-600 active:border-green-600 active:ring active:ring-green-100 dark:ring-green-400/20";

    case "red":
      if (props.variant === "soft") {
        return "text-red-500 btn bg-red-100 hover:text-white hover:bg-red-600 focus:text-white focus:bg-red-600 focus:ring focus:ring-red-100 active:text-white active:bg-red-600 active:ring active:ring-red-100 dark:bg-red-500/20 dark:text-red-500 dark:hover:bg-red-500 dark:hover:text-white dark:focus:bg-red-500 dark:focus:text-white dark:active:bg-red-500 dark:active:text-white dark:ring-red-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-red-500 btn border-red-500 hover:text-red-500 hover:bg-red-50 hover:border-red-600 focus:text-red-600 focus:bg-red-50 focus:border-red-600 active:text-red-600 active:bg-red-50 active:border-red-600 dark:bg-zink-700 dark:ring-red-400/20 dark:hover:bg-red-800/20 dark:focus:bg-red-800/20 dark:active:bg-red-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-red-500 btn border-red-500 hover:text-white hover:bg-red-600 hover:border-red-600 focus:text-white focus:bg-red-600 focus:border-red-600 focus:ring focus:ring-red-100 active:text-white active:bg-red-600 active:border-red-600 active:ring active:ring-red-100 dark:bg-zink-700 dark:hover:bg-red-500 dark:ring-red-400/20 dark:focus:bg-red-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-red-500 btn hover:text-red-500 hover:bg-red-100 focus:text-red-500 focus:bg-red-100 active:text-red-500 active:bg-red-100 dark:bg-zink-700 dark:hover:bg-red-500/10 dark:focus:bg-red-500/10 dark:active:bg-red-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-red-500 btn hover:text-red-700 focus:text-red-700 active:text-red-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-red-300 hover:text-red-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-red-400 hover:text-red-600";
      }
      return "text-white bg-red-500 border-red-500 hover:text-white hover:bg-red-600 hover:border-red-600 focus:text-white focus:bg-red-600 focus:border-red-600 focus:ring focus:ring-red-100 active:text-white active:bg-red-600 active:border-red-600 active:ring active:ring-red-100 dark:ring-red-400/20";

    case "orange":
      if (props.variant === "soft") {
        return "text-orange-500 btn bg-orange-100 hover:text-white hover:bg-orange-600 focus:text-white focus:bg-orange-600 focus:ring focus:ring-orange-100 active:text-white active:bg-orange-600 active:ring active:ring-orange-100 dark:bg-orange-500/20 dark:text-orange-500 dark:hover:bg-orange-500 dark:hover:text-white dark:focus:bg-orange-500 dark:focus:text-white dark:active:bg-orange-500 dark:active:text-white dark:ring-orange-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-orange-500 btn border-orange-500 hover:text-orange-500 hover:bg-orange-50 hover:border-orange-600 focus:text-orange-600 focus:bg-orange-50 focus:border-orange-600 active:text-orange-600 active:bg-orange-50 active:border-orange-600 dark:bg-zink-700 dark:ring-orange-400/20 dark:hover:bg-orange-800/20 dark:focus:bg-orange-800/20 dark:active:bg-orange-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-orange-500 btn border-orange-500 hover:text-white hover:bg-orange-600 hover:border-orange-600 focus:text-white focus:bg-orange-600 focus:border-orange-600 focus:ring focus:ring-orange-100 active:text-white active:bg-orange-600 active:border-orange-600 active:ring active:ring-orange-100 dark:bg-zink-700 dark:hover:bg-orange-500 dark:ring-orange-400/20 dark:focus:bg-orange-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-orange-500 btn hover:text-orange-500 hover:bg-orange-100 focus:text-orange-500 focus:bg-orange-100 active:text-orange-500 active:bg-orange-100 dark:bg-zink-700 dark:hover:bg-orange-500/10 dark:focus:bg-orange-500/10 dark:active:bg-orange-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-orange-500 btn hover:text-orange-700 focus:text-orange-700 active:text-orange-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-orange-300 hover:text-orange-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-orange-400 hover:text-orange-600";
      }
      return "text-white bg-orange-500 border-orange-500 hover:text-white hover:bg-orange-600 hover:border-orange-600 focus:text-white focus:bg-orange-600 focus:border-orange-600 focus:ring focus:ring-orange-100 active:text-white active:bg-orange-600 active:border-orange-600 active:ring active:ring-orange-100 dark:ring-orange-400/20";

    case "purple":
      if (props.variant === "soft") {
        return "text-purple-500 btn bg-purple-100 hover:text-white hover:bg-purple-600 focus:text-white focus:bg-purple-600 focus:ring focus:ring-purple-100 active:text-white active:bg-purple-600 active:ring active:ring-purple-100 dark:bg-purple-500/20 dark:text-purple-500 dark:hover:bg-purple-500 dark:hover:text-white dark:focus:bg-purple-500 dark:focus:text-white dark:active:bg-purple-500 dark:active:text-white dark:ring-purple-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-purple-500 btn border-purple-500 hover:text-purple-500 hover:bg-purple-50 hover:border-purple-600 focus:text-purple-600 focus:bg-purple-50 focus:border-purple-600 active:text-purple-600 active:bg-purple-50 active:border-purple-600 dark:bg-zink-700 dark:ring-purple-400/20 dark:hover:bg-purple-800/20 dark:focus:bg-purple-800/20 dark:active:bg-purple-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-purple-500 btn border-purple-500 hover:text-white hover:bg-purple-600 hover:border-purple-600 focus:text-white focus:bg-purple-600 focus:border-purple-600 focus:ring focus:ring-purple-100 active:text-white active:bg-purple-600 active:border-purple-600 active:ring active:ring-purple-100 dark:bg-zink-700 dark:hover:bg-purple-500 dark:ring-purple-400/20 dark:focus:bg-purple-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-purple-500 btn hover:text-purple-500 hover:bg-purple-100 focus:text-purple-500 focus:bg-purple-100 active:text-purple-500 active:bg-purple-100 dark:bg-zink-700 dark:hover:bg-purple-500/10 dark:focus:bg-purple-500/10 dark:active:bg-purple-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-purple-500 btn hover:text-purple-700 focus:text-purple-700 active:text-purple-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-purple-300 hover:text-purple-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-purple-400 hover:text-purple-600";
      }
      return "text-white bg-purple-500 border-purple-500 hover:text-white hover:bg-purple-600 hover:border-purple-600 focus:text-white focus:bg-purple-600 focus:border-purple-600 focus:ring focus:ring-purple-100 active:text-white active:bg-purple-600 active:border-purple-600 active:ring active:ring-purple-100 dark:ring-purple-400/20";

    case "sky":
      if (props.variant === "soft") {
        return "text-sky-500 btn bg-sky-100 hover:text-white hover:bg-sky-600 focus:text-white focus:bg-sky-600 focus:ring focus:ring-sky-100 active:text-white active:bg-sky-600 active:ring active:ring-sky-100 dark:bg-sky-500/20 dark:text-sky-500 dark:hover:bg-sky-500 dark:hover:text-white dark:focus:bg-sky-500 dark:focus:text-white dark:active:bg-sky-500 dark:active:text-white dark:ring-sky-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-sky-500 btn border-sky-500 hover:text-sky-500 hover:bg-sky-50 hover:border-sky-600 focus:text-sky-600 focus:bg-sky-50 focus:border-sky-600 active:text-sky-600 active:bg-sky-50 active:border-sky-600 dark:bg-zink-700 dark:ring-sky-400/20 dark:hover:bg-sky-800/20 dark:focus:bg-sky-800/20 dark:active:bg-sky-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-sky-500 btn border-sky-500 hover:text-white hover:bg-sky-600 hover:border-sky-600 focus:text-white focus:bg-sky-600 focus:border-sky-600 focus:ring focus:ring-sky-100 active:text-white active:bg-sky-600 active:border-sky-600 active:ring active:ring-sky-100 dark:bg-zink-700 dark:hover:bg-sky-500 dark:ring-sky-400/20 dark:focus:bg-sky-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-sky-500 btn hover:text-sky-500 hover:bg-sky-100 focus:text-sky-500 focus:bg-sky-100 active:text-sky-500 active:bg-sky-100 dark:bg-zink-700 dark:hover:bg-sky-500/10 dark:focus:bg-sky-500/10 dark:active:bg-sky-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-sky-500 btn hover:text-sky-700 focus:text-sky-700 active:text-sky-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-sky-300 hover:text-sky-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-sky-400 hover:text-sky-600";
      }
      return "text-white bg-sky-500 border-sky-500 hover:text-white hover:bg-sky-600 hover:border-sky-600 focus:text-white focus:bg-sky-600 focus:border-sky-600 focus:ring focus:ring-sky-100 active:text-white active:bg-sky-600 active:border-sky-600 active:ring active:ring-sky-100 dark:ring-sky-400/20";

    case "slate":
      if (props.variant === "soft") {
        return "text-slate-500 btn bg-slate-100 hover:text-white hover:bg-slate-600 focus:text-white focus:bg-slate-600 focus:ring focus:ring-slate-100 active:text-white active:bg-slate-600 active:ring active:ring-slate-100 dark:bg-slate-500/20 dark:text-slate-500 dark:hover:bg-slate-500 dark:hover:text-white dark:focus:bg-slate-500 dark:focus:text-white dark:active:bg-slate-500 dark:active:text-white dark:ring-slate-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-slate-500 btn border-slate-500 hover:text-slate-500 hover:bg-slate-50 hover:border-slate-600 focus:text-slate-600 focus:bg-slate-50 focus:border-slate-600 active:text-slate-600 active:bg-slate-50 active:border-slate-600 dark:bg-zink-700 dark:ring-slate-400/20 dark:hover:bg-slate-800/20 dark:focus:bg-slate-800/20 dark:active:bg-slate-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-slate-500 btn border-slate-500 hover:text-white hover:bg-slate-600 hover:border-slate-600 focus:text-white focus:bg-slate-600 focus:border-slate-600 focus:ring focus:ring-slate-100 active:text-white active:bg-slate-600 active:border-slate-600 active:ring active:ring-slate-100 dark:bg-zink-700 dark:hover:bg-slate-500 dark:ring-slate-400/20 dark:focus:bg-slate-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-slate-500 btn hover:text-slate-500 hover:bg-slate-100 focus:text-slate-500 focus:bg-slate-100 active:text-slate-500 active:bg-slate-100 dark:bg-zink-700 dark:hover:bg-slate-500/10 dark:focus:bg-slate-500/10 dark:active:bg-slate-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-slate-500 btn hover:text-slate-700 focus:text-slate-700 active:text-slate-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-slate-300 hover:text-slate-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-slate-400 hover:text-slate-600";
      }
      return "text-white bg-slate-500 border-slate-500 hover:text-white hover:bg-slate-600 hover:border-slate-600 focus:text-white focus:bg-slate-600 focus:border-slate-600 focus:ring focus:ring-slate-100 active:text-white active:bg-slate-600 active:border-slate-600 active:ring active:ring-slate-100 dark:ring-slate-400/20";
    case "light":
      return "text-slate-500 btn bg-slate-200 border-slate-200 hover:text-slate-600 hover:bg-slate-300 hover:border-slate-300 focus:text-slate-600 focus:bg-slate-300 focus:border-slate-300 focus:ring focus:ring-slate-100 active:text-slate-600 active:bg-slate-300 active:border-slate-300 active:ring active:ring-slate-100 dark:bg-zink-600 dark:hover:bg-zink-500 dark:border-zink-600 dark:hover:border-zink-500 dark:text-zink-200 dark:ring-zink-400/50";
    default:
      if (props.variant === "soft") {
        return "text-custom-500 btn bg-custom-100 hover:text-white hover:bg-custom-600 focus:text-white focus:bg-custom-600 focus:ring focus:ring-custom-100 active:text-white active:bg-custom-600 active:ring active:ring-custom-100 dark:bg-custom-500/20 dark:text-custom-500 dark:hover:bg-custom-500 dark:hover:text-white dark:focus:bg-custom-500 dark:focus:text-white dark:active:bg-custom-500 dark:active:text-white dark:ring-custom-400/20";
      } else if (props.variant === "dashed") {
        return "bg-white border-dashed text-custom-500 btn border-custom-500 hover:text-custom-500 hover:bg-custom-50 hover:border-custom-600 focus:text-custom-600 focus:bg-custom-50 focus:border-custom-600 active:text-custom-600 active:bg-custom-50 active:border-custom-600 dark:bg-zink-700 dark:ring-custom-400/20 dark:hover:bg-custom-800/20 dark:focus:bg-custom-800/20 dark:active:bg-custom-800/20";
      } else if (props.variant === "outlined") {
        return "bg-white text-custom-500 btn border-custom-500 hover:text-white hover:bg-custom-600 hover:border-custom-600 focus:text-white focus:bg-custom-600 focus:border-custom-600 focus:ring focus:ring-custom-100 active:text-white active:bg-custom-600 active:border-custom-600 active:ring active:ring-custom-100 dark:bg-zink-700 dark:hover:bg-custom-500 dark:ring-custom-400/20 dark:focus:bg-custom-500";
      } else if (props.variant === "ghost") {
        return "bg-white text-custom-500 btn hover:text-custom-500 hover:bg-custom-100 focus:text-custom-500 focus:bg-custom-100 active:text-custom-500 active:bg-custom-100 dark:bg-zink-700 dark:hover:bg-custom-500/10 dark:focus:bg-custom-500/10 dark:active:bg-custom-500/10";
      } else if (props.variant === "link") {
        return "bg-white border-white text-custom-500 btn hover:text-custom-700 focus:text-custom-700 active:text-custom-700 dark:bg-zink-700 dark:border-zink-700";
      } else if (props.variant === "plain-fill") {
        return "transition-all duration-200 ease-linear text-custom-300 hover:text-custom-100";
      } else if (props.variant === "plain-soft") {
        return "transition-all duration-200 ease-linear text-custom-400 hover:text-custom-600";
      }
      return "text-white bg-custom-500 border-custom-500 hover:text-white hover:bg-custom-600 hover:border-custom-600 focus:text-white focus:bg-custom-600 focus:border-custom-600 focus:ring focus:ring-custom-100 active:text-white active:bg-custom-600 active:border-custom-600 active:ring active:ring-custom-100 dark:ring-custom-400/20";
  }
});
</script>
<template>
  <button
    type="button"
    :class="`flex items-center justify-center btn  ${getClass} ${
      rounded ? 'rounded-full' : ''
    } ${icon ? 'size-[37.5px]' : 'h-fit'} ${classes} ${
      !border ? '!border-transparent' : ''
    }`"
  >
    <slot />
  </button>
</template>
