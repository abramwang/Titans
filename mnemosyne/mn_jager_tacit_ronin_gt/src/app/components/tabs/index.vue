<script setup lang="ts">
import { computed, onMounted, useSlots, ref, provide } from "vue";

const slots: any = useSlots();
const tabsSlots = ref<any>([]);
const emit = defineEmits(["update:modelValue"]);
const props = defineProps({
  modelValue: {
    type: String,
    required: true
  },
  hideDetails: {
    type: Boolean,
    default: false
  }
});

const activeTab = computed({
  get() {
    return props.modelValue;
  },
  set(newTab) {
    emit("update:modelValue", newTab);
  }
});

onMounted(() => {
  const slotDefault: any = slots.default();
  if (slotDefault) {
    slotDefault.forEach((item: any) => {
      tabsSlots.value.push({
        value: item.props.value,
        active: activeTab.value === item.props.value
      });
    });
  }
});

const onTabChange = (newTab: any) => {
  tabsSlots.value = tabsSlots.value.map((tab: any) => {
    if (tab.value === newTab) {
      activeTab.value = newTab;
      return {
        ...tab,
        active: true
      };
    }

    return {
      ...tab,
      active: false
    };
  });
};

provide("tabs", {
  activeTab: computed(() => activeTab.value),
  onTabChange
});
</script>
<template>
  <div :class="!hideDetails ? 'mb-5' : ''">
    <slot />
  </div>
</template>
