<script lang="ts" setup>
const getInputElement = (index: any) => {
  return document.getElementById("digit" + index + "-input");
};
const moveToNext = (index: any, event: any) => {
  const eventCode = event.which || event.keyCode;
  const input: any = getInputElement(index);
  if (input?.value.length === 1) {
    if (index !== 4) {
      getInputElement(index + 1)?.focus();
    } else {
      input.blur();
    }
  }
  if (eventCode === 8 && index !== 1) {
    getInputElement(index - 1)?.focus();
  }
};
</script>
<template>
  <div class="mt-8 text-center">
    <h4 class="mb-2 text-custom-500 dark:text-custom-500">Verify Email</h4>
    <p class="mb-8 text-slate-500 dark:text-zink-200 whitespace-normal">
      Please enter the <b class="font-medium">4</b> digit code sent to
      <b class="font-medium">tailwick@themesdesign.com</b>
    </p>
  </div>

  <form autocomplete="off" action="/" class="mt-8">
    <div class="grid grid-cols-1 gap-2 xl:grid-cols-4">
      <div v-for="item in 4" :key="'two-step-code-' + item">
        <label for="digit1-input" class="hidden">Digit {{ item }}</label>
        <input
          type="text"
          class="text-lg text-center form-input border-slate-200 dark:border-zink-500 focus:outline-none focus:border-custom-500 disabled:bg-slate-100 dark:disabled:bg-zink-600 disabled:border-slate-300 dark:disabled:border-zink-500 dark:disabled:text-zink-200 disabled:text-slate-500 dark:text-zink-100 dark:bg-zink-700 dark:focus:border-custom-800 placeholder:text-slate-400 dark:placeholder:text-zink-200"
          required
          placeholder="0"
          maxlength="1"
          :id="`digit${item}-input`"
          @keyup="moveToNext(item, $event)"
        />
      </div>
    </div>
    <div class="mt-10">
      <TButton type="submit" class="w-full"> Confirm </TButton>
    </div>
  </form>
</template>
