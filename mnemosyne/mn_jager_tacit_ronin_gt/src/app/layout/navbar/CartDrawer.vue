<script lang="ts" setup>
import { ref, computed, watch, onMounted } from "vue";
import { cartItems } from "@/app/layout/navbar/utils";
import { CartItemType } from "@/app/layout/navbar/type";
import { X } from "lucide-vue-next";
import { v4 as uuidv4 } from "uuid";

const emit = defineEmits(["updateCount", "hideCart"]);
const mappedData = cartItems.map((item) => {
  return {
    ...item,
    id: uuidv4()
  };
});

const cartData = ref<CartItemType[]>(mappedData);
const cartItemCount = computed(() => {
  return cartData.value.length;
});

watch(cartItemCount, (newVal: number) => {
  emit("updateCount", newVal);
});

onMounted(() => {
  emit("updateCount", cartItemCount.value);
});

const onRemove = (item: CartItemType) => {
  cartData.value = cartData.value.filter((data) => {
    return data.id !== item.id;
  });
};

const getSubTotal = (): any => {
  let count = 0;
  cartData.value.forEach((item) => {
    count += item.count * item.price;
  });
  return count.toFixed(2);
};

const getFinalTotal = () => {
  const subTotal = getSubTotal();
  if (subTotal > 0) {
    let count: number = getSubTotal() - 476 + 89;
    return (count - (count * 12.5) / 100).toFixed(2);
  }
  return 0;
};

const hideCart = () => {
  emit("hideCart");
};
</script>
<template>
  <div
    class="px-4 py-3 text-sm text-green-500 border border-transparent bg-green-50 dark:bg-green-400/20"
  >
    <span class="font-bold underline">TAILWICK50</span> Coupon code applied
    successfully.
  </div>
  <div class="h-full">
    <div
      v-if="!cartData.length"
      class="h-full flex flex-col items-center justify-center"
    >
      <div class="text-lg font-bold">Your cart is empty!</div>
      <div class="text-sm text-slate-500">
        Firstly add some product to cart.
      </div>
    </div>

    <simplebar
      v-if="cartData.length"
      class="h-[calc(100vh_-_370px)] p-4 product-list"
    >
      <div class="flex flex-col gap-4">
        <div
          v-for="(item, index) in cartData"
          :key="'cart-item-' + index"
          class="flex gap-2 product"
        >
          <div
            class="flex items-center justify-center size-12 rounded-md bg-slate-100 shrink-0 dark:bg-zink-500"
          >
            <img :src="item.src" :alt="'cart-item-' + item.title" class="h-8" />
          </div>
          <div class="overflow-hidden grow">
            <div class="ltr:float-right rtl:float-left">
              <button
                class="transition-all duration-150 ease-linear text-slate-500 dark:text-zink-200 hover:text-red-500 dark:hover:text-red-500"
              >
                <X class="size-4" @click="onRemove(item)" />
              </button>
            </div>
            <a
              href="#!"
              class="transition-all duration-200 ease-linear hover:text-custom-500"
            >
              <h6 class="mb-1 text-15">{{ item.title }}</h6>
            </a>
            <div class="flex items-center mb-3">
              <h5 class="text-base product-price">
                $<span>{{ item.price }}</span>
              </h5>
              <div
                class="font-normal rtl:mr-1 ltr:ml-1 text-slate-500 dark:text-zink-200"
              >
                ({{ item.type }})
              </div>
            </div>
            <div class="flex items-center justify-between gap-3">
              <TNumberInputSpinner
                v-model="item.count"
                @onAdd="item.count++"
                @onReduce="item.count--"
              />
              <h6 class="product-line-price">
                ${{ (item.count * item.price).toFixed(2) }}
              </h6>
            </div>
          </div>
        </div>
      </div>
    </simplebar>
    <div
      v-if="getSubTotal() > 0"
      class="p-4 border-t border-slate-200 dark:border-zink-500"
    >
      <table class="w-full mb-3">
        <tbody class="table-total">
          <tr>
            <td class="py-2">Sub Total :</td>
            <td class="text-right cart-subtotal">${{ getSubTotal() }}</td>
          </tr>
          <tr>
            <td class="py-2">
              Discount <span class="text-muted">(TAILWICK50)</span>:
            </td>
            <td class="text-right cart-discount">-$476.00</td>
          </tr>
          <tr>
            <td class="py-2">Shipping Charge :</td>
            <td class="text-right cart-shipping">$89.00</td>
          </tr>
          <tr>
            <td class="py-2">Estimated Tax (12.5%) :</td>
            <td class="text-right cart-tax">$70.62</td>
          </tr>
          <tr class="font-semibold">
            <td class="py-2">Total :</td>
            <td class="text-right cart-total">${{ getFinalTotal() }}</td>
          </tr>
        </tbody>
      </table>
      <div class="flex items-center justify-between gap-3">
        <router-link
          to="#"
          class="w-full text-white btn bg-slate-500 border-slate-500 hover:text-white hover:bg-slate-600 hover:border-slate-600 focus:text-white focus:bg-slate-600 focus:border-slate-600 focus:ring focus:ring-slate-100 active:text-white active:bg-slate-600 active:border-slate-600 active:ring active:ring-slate-100 dark:ring-slate-400/10"
          @click.prevent="hideCart"
        >
          Continue Shopping
        </router-link>
        <router-link
          to="#"
          class="w-full text-white bg-red-500 border-red-500 btn hover:text-white hover:bg-red-600 hover:border-red-600 focus:text-white focus:bg-red-600 focus:border-red-600 focus:ring focus:ring-red-100 active:text-white active:bg-red-600 active:border-red-600 active:ring active:ring-red-100 dark:ring-custom-400/20"
          @click.prevent="hideCart"
        >
          Checkout
        </router-link>
      </div>
    </div>
  </div>
</template>
