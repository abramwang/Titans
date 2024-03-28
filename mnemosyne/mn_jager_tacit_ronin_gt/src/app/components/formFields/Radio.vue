<script lang="ts" setup>
import { PropType, computed } from "vue";

const props = defineProps({
  color: {
    type: String,
    default: "custom"
  },
  checked: {
    type: Boolean,
    default: false
  },
  id: {
    type: String,
    default: ""
  },
  variant: {
    type: String as PropType<"bordered" | "soft" | "outlined">,
    default: ""
  },
  squared: {
    type: Boolean,
    default: false
  }
});

const variant = props.variant;

const getColorsClass = computed(() => {
  switch (props.color) {
    case "custom":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-custom-500 checked:border-custom-500 dark:checked:bg-custom-400 dark:checked:border-custom-400";
      } else if (variant === "soft") {
        return "border-custom-200 bg-custom-50 checked:bg-custom-500 checked:border-custom-500 dark:border-custom-800 dark:bg-custom-500/20 dark:checked:bg-custom-500 dark:checked:border-custom-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-custom-500 checked:border-custom-500 dark:after:text-custom-500 dark:checked:border-custom-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-custom-500 checked:border-custom-500 dark:checked:bg-custom-500 dark:checked:border-custom-500";
    case "yellow":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-yellow-500 checked:border-yellow-500 dark:checked:bg-yellow-400 dark:checked:border-yellow-400";
      } else if (variant === "soft") {
        return "border-yellow-200 bg-yellow-50 checked:bg-yellow-500 checked:border-yellow-500 dark:border-yellow-800 dark:bg-yellow-500/20 dark:checked:bg-yellow-500 dark:checked:border-yellow-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-yellow-500 checked:border-yellow-500 dark:after:text-yellow-500 dark:checked:border-yellow-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-yellow-500 checked:border-yellow-500 dark:checked:bg-yellow-500 dark:checked:border-yellow-500";
    case "green":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-green-500 checked:border-green-500 dark:checked:bg-green-400 dark:checked:border-green-400";
      } else if (variant === "soft") {
        return "border-green-200 bg-green-50 checked:bg-green-500 checked:border-green-500 dark:border-green-800 dark:bg-green-500/20 dark:checked:bg-green-500 dark:checked:border-green-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-green-500 checked:border-green-500 dark:after:text-green-500 dark:checked:border-green-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-green-500 checked:border-green-500 dark:checked:bg-green-500 dark:checked:border-green-500";
    case "red":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-red-500 checked:border-red-500 dark:checked:bg-red-400 dark:checked:border-red-400";
      } else if (variant === "soft") {
        return "border-red-200 bg-red-50 checked:bg-red-500 checked:border-red-500 dark:border-red-800 dark:bg-red-500/20 dark:checked:bg-red-500 dark:checked:border-red-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-red-500 checked:border-red-500 dark:after:text-red-500 dark:checked:border-red-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-red-500 checked:border-red-500 dark:checked:bg-red-500 dark:checked:border-red-500";
    case "orange":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-orange-500 checked:border-orange-500 dark:checked:bg-orange-400 dark:checked:border-orange-400";
      } else if (variant === "soft") {
        return "border-orange-200 bg-orange-50 checked:bg-orange-500 checked:border-orange-500 dark:border-orange-800 dark:bg-orange-500/20 dark:checked:bg-orange-500 dark:checked:border-orange-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-orange-500 checked:border-orange-500 dark:after:text-orange-500 dark:checked:border-orange-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-orange-500 checked:border-orange-500 dark:checked:bg-orange-500 dark:checked:border-orange-500";
    case "purple":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-purple-500 checked:border-purple-500 dark:checked:bg-purple-400 dark:checked:border-purple-400";
      } else if (variant === "soft") {
        return "border-purple-200 bg-purple-50 checked:bg-purple-500 checked:border-purple-500 dark:border-purple-800 dark:bg-purple-500/20 dark:checked:bg-purple-500 dark:checked:border-purple-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-purple-500 checked:border-purple-500 dark:after:text-purple-500 dark:checked:border-purple-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-purple-500 checked:border-purple-500 dark:checked:bg-purple-500 dark:checked:border-purple-500";
    case "sky":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-sky-500 checked:border-sky-500 dark:checked:bg-sky-400 dark:checked:border-sky-400";
      } else if (variant === "soft") {
        return "border-sky-200 bg-sky-50 checked:bg-sky-500 checked:border-sky-500 dark:border-sky-800 dark:bg-sky-500/20 dark:checked:bg-sky-500 dark:checked:border-sky-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-sky-500 checked:border-sky-500 dark:after:text-sky-500 dark:checked:border-sky-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-sky-500 checked:border-sky-500 dark:checked:bg-sky-500 dark:checked:border-sky-500";
    case "slate":
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-slate-500 checked:border-slate-500 dark:checked:bg-slate-400 dark:checked:border-slate-400";
      } else if (variant === "soft") {
        return "border-slate-200 bg-slate-50 checked:bg-slate-500 checked:border-slate-500 dark:border-slate-800 dark:bg-slate-500/20 dark:checked:bg-slate-500 dark:checked:border-slate-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-slate-500 checked:border-slate-500 dark:after:text-slate-500 dark:checked:border-slate-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-slate-500 checked:border-slate-500 dark:checked:bg-slate-500 dark:checked:border-slate-500";

    default:
      if (variant === "bordered") {
        return "bg-white border dark:bg-zink-700 border-slate-200 dark:border-zink-500 checked:bg-custom-500 checked:border-custom-500 dark:checked:bg-custom-400 dark:checked:border-custom-400";
      } else if (variant === "soft") {
        return "border-custom-200 bg-custom-50 checked:bg-custom-500 checked:border-custom-500 dark:border-custom-800 dark:bg-custom-500/20 dark:checked:bg-custom-500 dark:checked:border-custom-500";
      } else if (variant === "outlined") {
        return "bg-white border border-slate-200 dark:bg-zink-700 dark:border-zink-500 arrow-none relative after:absolute outlined-radio after:text-[8px] after:top-[3px] after:left-[3px] after:font-remix after:leading-none after:opacity-0 checked:after:opacity-100 after:text-custom-500 checked:border-custom-500 dark:after:text-custom-500 dark:checked:border-custom-800";
      }
      return "bg-slate-100 border-slate-200 dark:bg-zink-600 dark:border-zink-500 checked:bg-custom-500 checked:border-custom-500 dark:checked:bg-custom-500 dark:checked:border-custom-500";
  }
});
</script>
<template>
  <div class="flex items-center gap-2">
    <input
      class="size-4 border appearance-none cursor-pointer"
      :class="`${getColorsClass} ${squared ? 'rounded-sm' : 'rounded-full'}`"
      type="radio"
      :checked="checked"
      :id="id"
    />
    <label :for="id" class="align-middle select-none cursor-pointer">
      <slot />
    </label>
  </div>
</template>

<style lang="scss">
.outlined-radio::after {
  content: "\eb7c" !important;
}
</style>
