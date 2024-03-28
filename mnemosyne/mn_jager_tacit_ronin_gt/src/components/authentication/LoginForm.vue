<script lang="ts" setup>
import { computed, ref } from "vue";
import { socialMedias } from "@/components/authentication/utils.ts";
import {
  fakeBackendService,
  firebaseService
} from "@/app/service/httpService/httpServiceProvider.ts";
import appConfigs from "@/app/appConfig.ts";
import { UserType } from "@/app/service/httpService/types.ts";
import { useRouter } from "vue-router";
import { LAYOUT_TYPES } from "@/layouts/types.ts";
import { Mail, Smartphone } from "lucide-vue-next";

const { COVER, BOXED, MODERN } = LAYOUT_TYPES;
const props = defineProps({
  layout: {
    type: String,
    default: LAYOUT_TYPES.BASIC
  }
});
const router = useRouter();
const auth = appConfigs.auth;

const modernLoginType = ref("emailLogin");
const tabClass = `inline-block px-4 w-full py-2 text-base transition-all duration-300 ease-linear rounded-md text-slate-500 bg-slate-100 dark:text-zink-200 dark:bg-zink-600 border border-transparent group-[.active]:bg-custom-500 dark:group-[.active]:bg-custom-500 group-[.active]:text-white dark:group-[.active]:text-white hover:text-custom-500 dark:hover:text-custom-500 active:text-custom-500 dark:active:text-custom-500 -mb-[1px]`;

const form = ref({
  username: { value: "admin", isValid: true },
  password: { value: "123456", isValid: true },
  phone: { value: "", isValid: true },
  isRemember: true
});

const isSubmitted = ref(false);
const isSucceed = ref(false);
const errorMsg = ref("");

const isValidForm = computed(() => {
  const { username, password, isRemember, phone } = form.value;
  return username.isValid && password.isValid && isRemember && phone.isValid;
});

const getTitleColor = computed(() => {
  if (props.layout === BOXED || props.layout === COVER) {
    return "text-purple-500 dark:text-purple-500";
  }
  return "text-custom-500 dark:text-custom-500";
});
const onSignIn = async () => {
  isSubmitted.value = true;
  errorMsg.value = "";
  if (isValidForm.value) {
    try {
      const { password, username, phone } = form.value;
      const payload: UserType = {
        password: password.value,
        username: username.value,
        phone: phone.value
      };

      if (auth === "fakebackend") {
        const data = await fakeBackendService.login(payload);
        if (data) {
          isSucceed.value = true;
          setTimeout(() => {
            router.push({ path: "/" });
          }, 2000);
        }
      } else if (auth === "firebase") {
        await firebaseService.loginUser({
          ...payload,
          email: payload.username
        });
      }
    } catch (error: any) {
      errorMsg.value = error.message;
    }
  }
};
</script>
<template>
  <div v-if="layout === MODERN" class="lg:w-[20rem] mx-auto mt-10">
    <TTabs
      v-model="modernLoginType"
      class="flex flex-wrap w-full gap-2 text-sm font-medium text-center nav-tabs"
    >
      <TTab grow value="emailLogin" :tabClass="tabClass">
        <Mail class="inline-block size-4 mr-1" />
        Email
      </TTab>
      <TTab grow value="phoneLogin" :tabClass="tabClass">
        <Smartphone class="inline-block size-4 mr-1" />
        Phone
      </TTab>
    </TTabs>
  </div>
  <div v-else class="text-center">
    <h4 class="mb-1" :class="getTitleColor">Welcome Back !</h4>
    <p class="text-slate-500 dark:text-zink-200">
      Sign in to continue to Tailwick.
    </p>
  </div>

  <div :class="layout === MODERN ? 'lg:w-[25rem] mx-auto' : ''" class="mt-10">
    <div
      v-show="isSucceed"
      class="text-wrap px-4 py-3 mb-3 text-sm text-green-500 border border-green-200 rounded-md bg-green-50 dark:bg-green-400/20 dark:border-green-500/50"
    >
      You have <b>successfully</b> signed in.
    </div>
    <TAlert v-show="errorMsg" color="red" bordered class="mb-3">
      {{ errorMsg }}
    </TAlert>
    <div>
      <TValidationInputField
        v-if="modernLoginType === 'emailLogin'"
        v-model="form.username"
        label="UserName/ Email ID"
        :rules="['required']"
        :submit="isSubmitted"
        placeholder="Enter username or email"
      />
      <TValidationInputField
        v-else
        v-model="form.phone"
        type="number"
        label="Phone Number"
        :rules="['required']"
        :submit="isSubmitted"
        placeholder="Enter Phone"
      />
      <TValidationInputField
        v-model="form.password"
        label="Password"
        type="password"
        :rules="['required']"
        :submit="isSubmitted"
        placeholder="Enter password"
      />
      <TCheckbox v-model="form.isRemember" id="sign-in-remember">
        <span class="text-base font-medium"> Remember me </span>
      </TCheckbox>
      <div
        v-if="isSubmitted && !form.isRemember"
        class="mt-1 text-sm text-red-500"
      >
        Please check the "Remember me" before submitting the form.
      </div>
    </div>
    <div class="mt-10">
      <TButton class="w-full" @click="onSignIn"> Sign In </TButton>
    </div>
    <div
      class="relative text-center my-9 before:absolute before:top-3 before:left-0 before:right-0 before:border-t before:border-t-slate-200 dark:before:border-t-zink-500"
    >
      <h5
        class="inline-block px-2 py-0.5 text-sm bg-white text-slate-500 dark:bg-zink-600 dark:text-zink-200 rounded relative"
      >
        Sign In with
      </h5>
    </div>

    <div class="flex flex-wrap justify-center gap-2">
      <TButton
        v-for="(item, index) in socialMedias"
        :key="'login-social-media-' + index"
        icon
        :variant="layout === MODERN ? 'soft' : ''"
        class="p-0"
        :color="item.color"
      >
        <component :is="item.icon" class="size-4" />
      </TButton>
    </div>

    <div class="mt-10 text-center">
      <p class="mb-0 text-slate-500 dark:text-zink-200">
        Don't have an account ?
        <router-link
          :to="`/register/${layout}`"
          class="font-semibold underline transition-all duration-150 ease-linear text-slate-500 dark:text-zink-200 hover:text-custom-500 dark:hover:text-custom-500"
        >
          SignUp
        </router-link>
      </p>
    </div>
  </div>
</template>
