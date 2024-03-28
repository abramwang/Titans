<script lang="ts" setup>
import { X } from "lucide-vue-next";
import { computed, ref } from "vue";
const emit = defineEmits(["update:modelValue", "onAddUpdate"]);

const props = defineProps({
  modelValue: {
    type: Boolean,
    default: false
  },
  formData: {
    type: Object,
    default: {}
  }
});

const preparedData = ref(props.formData);

const errorMsg = ref("");
const createEditModal = computed({
  get() {
    return props.modelValue;
  },
  set(newVal: boolean) {
    emit("update:modelValue", newVal);
  }
});

const isEdit = computed(
  () => preparedData.value.id && preparedData.value.id > 0
);

const onAddUpdate = () => {
  const data = [];
  for (const key in preparedData.value) {
    data.push(preparedData.value[key]);
  }
  const isEmptyData = data.findIndex((item) => item === "");
  if (isEmptyData !== -1) {
    errorMsg.value = "Please Fill all fields!";
    return;
  } else {
    errorMsg.value = "";
    emit("onAddUpdate", preparedData.value);
  }
};
</script>
<template>
  <TModal v-model="createEditModal">
    <template #content>
      <div
        class="flex items-center justify-between p-4 border-b border-slate-200 dark:border-zink-500"
      >
        <h5 class="text-16" id="exampleModalLabel">
          {{ isEdit ? "Update Customer" : "Add Customer" }}
        </h5>
        <TButton
          icon
          variant="plain-soft"
          class="p-0"
          color="slate"
          @click="createEditModal = false"
        >
          <X />
        </TButton>
      </div>
      <div class="p-4">
        <div class="text-sm text-red-500 mb-3">{{ errorMsg }}</div>
        <TInputField
          v-model="preparedData.customer_name"
          label="Customer name"
          placeholder="Enter your name"
          required
        />
        <TInputField
          v-model="preparedData.email"
          label="Email"
          placeholder="Enter email"
          required
        />
        <TInputField
          v-model="preparedData.phone"
          label="Phone Number"
          placeholder="Enter your name"
          required
        />
        <TFlatPicker
          v-model="preparedData.date"
          label="Joining Date"
          required
        />
        <div>
          <label class="mb-2 text-base font-medium">Status</label>
          <span class="text-red-500">*</span>
        </div>
        <TList
          v-model="preparedData.status"
          :items="['Active', 'Block']"
          placement="bottom-start"
          class="!block relative"
          @onSelect="preparedData.status = $event"
        >
          <template #title>
            <div
              class="form-input border-slate-200 dark:border-zink-500 focus:outline-none focus:border-custom-500 disabled:bg-slate-100 dark:disabled:bg-zink-600 disabled:border-slate-300 dark:disabled:border-zink-500 dark:disabled:text-zink-200 disabled:text-slate-500 dark:text-zink-100 dark:bg-zink-700 dark:focus:border-custom-800 placeholder:text-slate-400 dark:placeholder:text-zink-200 choices__input w-full"
            >
              {{ preparedData.status || "Select Value" }}
            </div>
          </template>
        </TList>
        <div class="flex gap-2 justify-end mt-3">
          <TButton color="slate" @click="createEditModal = false">
            Close
          </TButton>
          <TButton color="green" @click="onAddUpdate">
            {{ isEdit ? "Update" : "Add Customer" }}
          </TButton>
        </div>
      </div>
    </template>
  </TModal>
</template>
