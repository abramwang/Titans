<script lang="ts" setup>
import Picker from "@simonwep/pickr";
import { onMounted, type PropType, onBeforeUnmount, ref } from "vue";

const monolithPicker = ref();
const classicPicker = ref();
const nanoPicker = ref();

const props = defineProps({
  theme: {
    type: String as PropType<"monolith" | "classic" | "nano">,
    default: ""
  },
  config: {
    type: Object,
    default: {
      default: "rgba(63, 81, 181, 0.8)",
      swatches: [
        "rgba(244, 67, 54, 1)",
        "rgba(233, 30, 99, 0.95)",
        "rgba(156, 39, 176, 0.9)",
        "rgba(103, 58, 183, 0.85)",
        "rgba(63, 81, 181, 0.8)",
        "rgba(33, 150, 243, 0.75)",
        "rgba(3, 169, 244, 0.7)"
      ],
      defaultRepresentation: "HEXA",
      components: {
        preview: true,
        opacity: true,
        hue: true,
        interaction: {
          hex: false,
          rgba: false,
          hsva: false,
          input: true,
          clear: true,
          save: true
        }
      }
    }
  }
});

const theme = props.theme;

onMounted(() => {
  if (theme === "monolith") {
    initMonolith();
  } else if (theme === "classic") {
    initClassic();
  } else if (theme === "nano") {
    initNano();
  }
});

onBeforeUnmount(() => {
  monolithPicker.value?.destroyAndRemove();
  classicPicker.value?.destroyAndRemove();
  nanoPicker.value?.destroyAndRemove();
});

const initMonolith = () => {
  monolithPicker.value = Picker.create({
    el: ".monolith-colorpicker",
    theme: "monolith",
    ...props.config
  });
};

const initClassic = () => {
  classicPicker.value = Picker.create({
    el: ".classic-colorpicker",
    theme: "classic",
    ...props.config
  });
};

const initNano = () => {
  nanoPicker.value = Picker.create({
    el: ".nano-colorpicker",
    theme: "nano",
    ...props.config
  });
};
</script>
<template>
  <div v-if="theme === 'monolith'" class="monolith-colorpicker"></div>
  <div v-if="theme === 'classic'" class="classic-colorpicker"></div>
  <div v-if="theme === 'nano'" class="nano-colorpicker"></div>
</template>
