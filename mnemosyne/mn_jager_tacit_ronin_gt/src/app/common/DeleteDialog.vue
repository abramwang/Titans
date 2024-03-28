<script lang="ts" setup>
import { computed } from "vue";
import { X } from "lucide-vue-next";
const emit = defineEmits(["update:modelValue", "handleSubmit"]);

const props = defineProps({
  modelValue: {
    type: Boolean,
    default: false
  }
});

const showModal = computed({
  get() {
    return props.modelValue;
  },
  set(newVal: Boolean) {
    emit("update:modelValue", newVal);
  }
});

const onConfirm = () => {
  emit("handleSubmit");
};
</script>
<template>
  <TModal v-model="showModal" size="x-small">
    <template #content>
      <div
        class="max-h-[calc(theme('height.screen')_-_180px)] overflow-y-auto px-6 py-8 relative"
      >
        <div class="float-right">
          <button
            class="p-0 shrink-0 transition-all duration-200 ease-linear text-slate-500 hover:text-red-500"
            @click="showModal = false"
          >
            <X class="size-5" />
          </button>
        </div>
        <img
          src="@/assets/images/delete.png"
          alt=""
          class="block h-12 mx-auto"
        />
        <div class="mt-5 text-center">
          <h5 class="mb-1">Are you sure?</h5>
          <p class="text-slate-500 dark:text-zink-200">
            Are you certain you want to delete this record?
          </p>
          <div class="flex justify-center gap-2 mt-6">
            <button
              class="bg-white text-slate-500 btn hover:text-slate-500 hover:bg-slate-100 focus:text-slate-500 focus:bg-slate-100 active:text-slate-500 active:bg-slate-100 dark:bg-zink-600 dark:hover:bg-slate-500/10 dark:focus:bg-slate-500/10 dark:active:bg-slate-500/10"
              @click="showModal = false"
            >
              Cancel
            </button>
            <TButton color="red" @click="onConfirm"> Yes, Delete It! </TButton>
          </div>
        </div>
      </div>
    </template>
  </TModal>
</template>
