<script lang="ts" setup>
import { type PropType } from "vue";
import { toast } from "vue3-toastify";

const props = defineProps({
  variant: {
    type: String as PropType<
      "default" | "success" | "warning" | "error" | "info"
    >,
    default: ""
  },
  position: {
    type: String as PropType<
      | "top-left"
      | "top-center"
      | "top-right"
      | "bottom-left"
      | "bottom-center"
      | "bottom-right"
    >,
    default: ""
  },
  offset: {
    type: Boolean,
    default: false
  },
  showClose: {
    type: Boolean,
    default: false
  },
  showProgress: {
    type: Boolean,
    default: false
  }
});

const notify = () => {
  const { variant, position, offset, showClose, showProgress } = props;
  if (variant === "default") {
    toast("Welcome Back! This is a Toast Notification", {
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (variant === "success") {
    toast.success("Your application was successfully sent", {
      closeButton: false,
      toastClassName: "bg-green-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (variant === "warning") {
    toast.warn("Warning ! Something went wrong try again", {
      closeButton: false,
      toastClassName: "bg-yellow-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (variant === "error") {
    toast.error("Error ! An error occurred.", {
      closeButton: false,
      toastClassName: "bg-red-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (variant === "info") {
    toast.info("Info Notification !", {
      closeButton: false,
      toastClassName: "bg-sky-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  }

  if (position === "top-left") {
    toast("Welcome Back ! This is a Toast Notification", {
      position: toast.POSITION.TOP_LEFT,
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (position === "top-center") {
    toast("Welcome Back ! This is a Toast Notification", {
      position: toast.POSITION.TOP_CENTER,
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (position === "top-right") {
    toast("Welcome Back ! This is a Toast Notification", {
      position: toast.POSITION.TOP_RIGHT,
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (position === "bottom-left") {
    toast("Welcome Back ! This is a Toast Notification", {
      position: toast.POSITION.BOTTOM_LEFT,
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (position === "bottom-center") {
    toast("Welcome Back ! This is a Toast Notification", {
      position: toast.POSITION.BOTTOM_CENTER,
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  } else if (position === "bottom-right") {
    toast("Welcome Back ! This is a Toast Notification", {
      position: toast.POSITION.BOTTOM_RIGHT,
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000
    });
  }

  if (offset) {
    toast("Welcome Back ! This is a Toast Notification", {
      closeButton: false,
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      icon: false,
      autoClose: 1000,
      toastStyle: {
        top: "50px",
        right: "50px"
      }
    });
  }

  if (showClose) {
    toast("Welcome Back ! This is a Toast Notification", {
      toastClassName: "bg-custom-500 text-white",
      hideProgressBar: true,
      autoClose: 1000
    });
  }

  if (showProgress) {
    toast("Welcome Back ! This is a Toast Notification");
  }
};
</script>
<template>
  <slot :onClick="notify" />
</template>

<style lang="scss">
.Toastify__toast {
  margin: 6px;
  padding: 5px;
  min-height: 40px;
  height: 55px;
}
.Toastify__toast-body {
  min-height: 40px;
  height: 45px;
  margin: 0;
  padding: 0 1rem;
}

.Toastify__close-button {
  color: white;
  padding: 10px;
  display: flex;
  margin-top: auto;
  margin-bottom: auto;
}
</style>
