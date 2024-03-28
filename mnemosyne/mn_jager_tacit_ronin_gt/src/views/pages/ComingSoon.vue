<script lang="ts" setup>
import { onMounted, ref } from "vue";

const time = ref(10);

onMounted(() => {
  document.body.setAttribute(
    "class",
    "flex items-center justify-center min-h-screen py-16 bg-cover bg-auth-pattern dark:bg-auth-pattern-dark font-public"
  );
});

onMounted(() => {
  setInterval(() => {
    if (time.value > 0) {
      time.value--;
    }
  }, 1000);
});
</script>
<template>
  <div id="countDownText">
    <div v-if="time > 0">
      <div class="mt-8 text-center">
        <h4 class="mb-2 text-purple-500 dark:text-purple-500">
          Coming Soon ...
        </h4>
        <p class="text-slate-500 dark:text-zink-200">
          We'll be here in a brief moment.
        </p>
      </div>

      <div>
        <div class="mt-8">
          <div class="flex items-center justify-center mt-14 gap-1 flex-wrap">
            <div
              class="flex flex-col items-center justify-center size-24 p-2 text-center bg-white rounded dark:bg-zink-600/50 countdownlist-item"
            >
              <div class="count-title text-slate-500 dark:text-zink-200">
                Days
              </div>
              <div
                class="mt-2 text-2xl font-semibold count-num dark:text-zink-50"
              >
                0
              </div>
            </div>
            <div
              class="flex flex-col items-center justify-center size-24 p-2 text-center bg-white rounded dark:bg-zink-600/50 countdownlist-item"
            >
              <div class="count-title text-slate-500 dark:text-zink-200">
                Hours
              </div>
              <div
                class="mt-2 text-2xl font-semibold count-num dark:text-zink-50"
              >
                0
              </div>
            </div>
            <div
              class="flex flex-col items-center justify-center size-24 p-2 text-center bg-white rounded dark:bg-zink-600/50 countdownlist-item"
            >
              <div class="count-title text-slate-500 dark:text-zink-200">
                Minutes
              </div>
              <div
                class="mt-2 text-2xl font-semibold count-num dark:text-zink-50"
              >
                0
              </div>
            </div>
            <div
              class="flex flex-col items-center justify-center size-24 p-2 text-center bg-white rounded dark:bg-zink-600/50 countdownlist-item"
            >
              <div class="count-title text-slate-500 dark:text-zink-200">
                seconds
              </div>
              <div
                class="mt-2 text-2xl font-semibold count-num dark:text-zink-50"
              >
                  {{ time }}
              </div>
            </div>
          </div>
        </div>

        <div class="mt-10 text-center">
          <h5 class="mb-2">Be alerted when our launch happens.</h5>
          <p class="mb-5 text-slate-500 dark:text-zink-200">
            Don't worry, we won't inundate your inbox 😊
          </p>
          <form action="#!" class="relative">
            <input
              type="email"
              id="inputEmail"
              class="py-2.5 form-input dark:bg-zink-600/50 border-slate-200 dark:border-zink-500 focus:outline-none focus:border-custom-500 disabled:bg-slate-100 dark:disabled:bg-zink-600 disabled:border-slate-300 dark:disabled:border-zink-500 dark:disabled:text-zink-200 disabled:text-slate-500 dark:text-zink-100 dark:bg-zink-700 dark:focus:border-custom-800 placeholder:text-slate-400 dark:placeholder:text-zink-200"
              placeholder="Enter your email"
              required
            />
            <button
              type="submit"
              class="px-2.5 py-1.5 text-sm text-white btn bg-custom-500 border-custom-500 hover:text-white hover:bg-custom-600 hover:border-custom-600 focus:text-white focus:bg-custom-600 focus:border-custom-600 focus:ring focus:ring-custom-100 active:text-white active:bg-custom-600 active:border-custom-600 active:ring active:ring-custom-100 dark:ring-custom-400/20 absolute top-[5px] right-1 rtl:left-1"
            >
              Send
            </button>
          </form>
        </div>
      </div>
    </div>
    <div v-else class="text-center">
      <i class="text-4xl text-green-500 ri-verified-badge-line"></i>
      <div class="mt-5 mb-4">
        <h4 class="mb-2">We've Launched our new website</h4>
        <p class="text-slate-500 dark:text-zink-200">
          Click the below button to visit our website.
        </p>
      </div>
      <div class="flex justify-center">
        <TButton>
          <router-link to="/"> Back to Home </router-link>
        </TButton>
      </div>
    </div>
  </div>
</template>
