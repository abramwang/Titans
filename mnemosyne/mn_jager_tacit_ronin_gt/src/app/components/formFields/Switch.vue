<script lang="ts" setup>
import { PropType, computed } from "vue";

const props = defineProps({
  color: {
    type: String,
    default: "custom"
  },
  squared: {
    type: Boolean,
    default: false
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
    type: String as PropType<"soft" | "outlined">
  },
  disabled: {
    type: Boolean,
    default: false
  },
  size: {
    type: String as PropType<"small" | "default" | "large">,
    default: "default"
  }
});

const variant = props.variant;
const size = props.size;

const inputColorClass = computed(() => {
  switch (props.color) {
    case "custom":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-custom-500 dark:checked:bg-custom-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-custom-100 dark:checked:border-custom-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-custom-500 dark:checked:bg-custom-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }

      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-custom-500 dark:checked:border-custom-500";
    case "yellow":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-yellow-500 dark:checked:bg-yellow-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-yellow-100 dark:checked:border-yellow-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-yellow-500 dark:checked:bg-yellow-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-yellow-500 dark:checked:border-yellow-500";
    case "green":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-green-500 dark:checked:bg-green-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-green-100 dark:checked:border-green-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-green-500 dark:checked:bg-green-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-green-500 dark:checked:border-green-500";
    case "red":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-red-500 dark:checked:bg-red-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-red-100 dark:checked:border-red-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-red-500 dark:checked:bg-red-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-red-500 dark:checked:border-red-500";
    case "orange":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-orange-500 dark:checked:bg-orange-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-orange-100 dark:checked:border-orange-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-orange-500 dark:checked:bg-orange-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-orange-500 dark:checked:border-orange-500";
    case "purple":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-purple-500 dark:checked:bg-purple-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-purple-100 dark:checked:border-purple-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-purple-500 dark:checked:bg-purple-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-purple-500 dark:checked:border-purple-500";
    case "sky":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-sky-500 dark:checked:bg-sky-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-sky-100 dark:checked:border-sky-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-sky-500 dark:checked:bg-sky-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-sky-500 dark:checked:border-sky-500";
    case "slate":
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-soft-500 dark:checked:bg-soft-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-soft-100 dark:checked:border-soft-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-slate-500 dark:checked:bg-slate-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-slate-500 dark:checked:border-slate-500";

    default:
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-600 bg-white/80 dark:bg-zink-400 peer/published checked:bg-custom-500 dark:checked:bg-custom-500 ltr:checked:right-0 rtl:checked:left-0 checked:border-custom-100 dark:checked:border-custom-900 arrow-none checked:bg-none";
      } else if (variant === "outlined") {
        return "!h-4 !w-4 top-0.5 rounded-full dark:border-zink-700 bg-slate-200 peer/published dark:bg-zink-500 checked:bg-custom-500 dark:checked:bg-custom-500 ltr:checked:right-0.5 rtl:checked:left-0.5 checked:left-auto checked:border-white dark:checked:border-zink-700 arrow-none";
      }
      return "border-slate-200 dark:border-zink-500 bg-white/80 dark:bg-zink-400 peer/published checked:bg-white dark:checked:bg-white ltr:checked:right-0 rtl:checked:left-0 checked:bg-none checked:border-custom-500 dark:checked:border-custom-500";
  }
});

const labelClasses = computed(() => {
  switch (props.color) {
    case "custom": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-custom-100 dark:peer-checked/published:bg-custom-900 peer-checked/published:border-custom-100 dark:peer-checked/published:border-custom-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-custom-500 dark:peer-checked/published:border-custom-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-custom-500 peer-checked/published:border-custom-500";
    }
    case "yellow": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-yellow-100 dark:peer-checked/published:bg-yellow-900 peer-checked/published:border-yellow-100 dark:peer-checked/published:border-yellow-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-yellow-500 dark:peer-checked/published:border-yellow-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-yellow-500 peer-checked/published:border-yellow-500";
    }
    case "green": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-green-100 dark:peer-checked/published:bg-green-900 peer-checked/published:border-green-100 dark:peer-checked/published:border-green-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-green-500 dark:peer-checked/published:border-green-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-green-500 peer-checked/published:border-green-500";
    }
    case "red": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-red-100 dark:peer-checked/published:bg-red-900 peer-checked/published:border-red-100 dark:peer-checked/published:border-red-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-red-500 dark:peer-checked/published:border-red-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-red-500 peer-checked/published:bg-red-500";
    }
    case "orange": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-orange-100 dark:peer-checked/published:bg-orange-900 peer-checked/published:border-orange-100 dark:peer-checked/published:border-orange-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-orange-500 dark:peer-checked/published:border-orange-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-orange-500 peer-checked/published:border-orange-500";
    }
    case "purple": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-purple-100 dark:peer-checked/published:bg-purple-900 peer-checked/published:border-purple-100 dark:peer-checked/published:border-purple-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-purple-500 dark:peer-checked/published:border-purple-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-purple-500 peer-checked/published:border-purple-500";
    }
    case "sky": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-sky-100 dark:peer-checked/published:bg-sky-900 peer-checked/published:border-sky-100 dark:peer-checked/published:border-sky-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-sky-500 dark:peer-checked/published:border-sky-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-sky-500 peer-checked/published:bg-sky-500";
    }
    case "slate": {
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-slate-100 dark:peer-checked/published:bg-slate-900 peer-checked/published:border-slate-100 dark:peer-checked/published:border-slate-900";
      } else if (variant === "outlined") {
        return "bg-white border rounded-full dark:bg-zink-700 border-slate-200 dark:border-zink-500 peer-checked/published:bg-white dark:peer-checked/published:bg-zink-700 peer-checked/published:border-slate-500 dark:peer-checked/published:border-slate-800";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-slate-500 peer-checked/published:border-slate-500";
    }
    default:
      if (variant === "soft") {
        return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:bg-custom-100 dark:peer-checked/published:bg-custom-900 peer-checked/published:border-custom-100 dark:peer-checked/published:border-custom-900";
      }
      return "border-slate-200 dark:border-zink-500 bg-slate-200 dark:bg-zink-600 peer-checked/published:defa-custom-500 peer-checked/published:defa-custom-500";
  }
});

const getInputSizeClass = computed(() => {
  if (size === "small") {
    return "size-4";
  } else if (size === "large") {
    return "size-6";
  }
  return "size-5";
});

const getLabelSizeClass = computed(() => {
  if (size === "small") {
    return "h-4";
  } else if (size === "large") {
    return "h-6";
  }
  return "h-5";
});
</script>
<template>
  <div class="flex items-center">
    <div
      class="relative inline-block align-middle transition duration-200 ease-in ltr:mr-2 rtl:ml-2"
      :class="{
        'w-8': size === 'small',
        'w-10': size === 'default',
        'w-12': size === 'large'
      }"
    >
      <input
        type="checkbox"
        :name="id"
        :id="id"
        class="absolute block transition duration-300 ease-linear border-2 appearance-none cursor-pointer arrow-none"
        :checked="checked"
        :class="`${inputColorClass} ${
          squared ? 'rounded-sm' : 'rounded-full'
        } ${getInputSizeClass}`"
        :disabled="disabled"
      />
      <label
        :for="id"
        class="block overflow-hidden duration-300 ease-linear border cursor-pointer cursor-pointer-transition"
        :class="`${labelClasses} ${
          squared ? 'rounded-sm' : 'rounded-full'
        } ${getLabelSizeClass}`"
      />
    </div>
    <label :for="id" class="inline-block text-base font-medium cursor-pointer">
      <slot />
    </label>
  </div>
</template>
