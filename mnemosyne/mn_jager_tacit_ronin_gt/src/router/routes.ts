import DefaultLayout from "@/layouts/Default.vue";
import { defineAsyncComponent } from "vue";
import { LAYOUT_TYPES } from "@/layouts/types.ts";

const BasicLayout = defineAsyncComponent(() => import("@/layouts/Basic.vue"));
const BoxedLayout = defineAsyncComponent(() => import("@/layouts/Boxed.vue"));
const CoverLayout = defineAsyncComponent(() => import("@/layouts/Cover.vue"));
const ModernLayout = defineAsyncComponent(() => import("@/layouts/Modern.vue"));

const accountRoutes = [
  {
    path: "/login",
    name: "SignInAccount",
    component: () => import("@/views/account/Login.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Sign In",
      authRequired: false,
      layout: BasicLayout
    }
  }
];

const dashboardRoutes = [
  {
    path: "/",
    name: "ECommerce",
    component: () => import("@/views/dashboard/Ecommerce.vue"),
    meta: {
      title: "ECommerce",
      authRequired: true,
      layout: DefaultLayout
    }
  },
];
const authRoutes = [
  {
    path: "/login/basic",
    name: "SignInBasic",
    component: () => import("@/views/authentication/Login.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Sign In",
      authRequired: false,
      layout: BasicLayout
    }
  },
  {
    path: "/login/cover",
    name: "SignInCover",
    component: () => import("@/views/authentication/Login.vue"),
    props: () => ({ layout: LAYOUT_TYPES.COVER }),
    meta: {
      title: "Sign In",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/login/boxed",
    name: "SignInBoxed",
    component: () => import("@/views/authentication/Login.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BOXED }),
    meta: {
      title: "Sign In",
      authRequired: false,
      layout: BoxedLayout
    }
  },
  {
    path: "/login/modern",
    name: "SignInModern",
    component: () => import("@/views/authentication/Login.vue"),
    props: () => ({ layout: LAYOUT_TYPES.MODERN }),
    meta: {
      title: "Sign In",
      authRequired: false,
      layout: ModernLayout
    }
  },
  {
    path: "/register/basic",
    name: "RegisterBasic",
    component: () => import("@/views/authentication/Register.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Register",
      authRequired: false,
      layout: BasicLayout
    }
  },
  {
    path: "/register/cover",
    name: "RegisterCover",
    component: () => import("@/views/authentication/Register.vue"),
    props: () => ({ layout: LAYOUT_TYPES.COVER }),
    meta: {
      title: "Register",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/register/boxed",
    name: "RegisterBoxed",
    component: () => import("@/views/authentication/Register.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BOXED }),
    meta: {
      title: "Register",
      authRequired: false,
      layout: BoxedLayout
    }
  },
  {
    path: "/register/modern",
    name: "RegisterModern",
    component: () => import("@/views/authentication/Register.vue"),
    props: () => ({ layout: LAYOUT_TYPES.MODERN }),
    meta: {
      title: "Register",
      authRequired: false,
      layout: ModernLayout
    }
  },
  {
    path: "/verify-email/basic",
    name: "VerifyEmailBasic",
    component: () => import("@/views/authentication/VerifyEmail.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Verify Email",
      authRequired: false,
      layout: BasicLayout
    }
  },
  {
    path: "/verify-email/cover",
    name: "VerifyEmailCover",
    component: () => import("@/views/authentication/VerifyEmail.vue"),
    props: () => ({ layout: LAYOUT_TYPES.COVER }),
    meta: {
      title: "Verify Email",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/verify-email/modern",
    name: "VerifyEmailModern",
    component: () => import("@/views/authentication/VerifyEmail.vue"),
    props: () => ({ layout: LAYOUT_TYPES.MODERN }),
    meta: {
      title: "Verify Email",
      authRequired: false,
      layout: ModernLayout
    }
  },
  {
    path: "/two-steps/basic",
    name: "TwoStepBasic",
    component: () => import("@/views/authentication/TwoStep.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Two Steps",
      authRequired: false,
      layout: BasicLayout
    }
  },
  {
    path: "/two-steps/cover",
    name: "TwoStepCover",
    component: () => import("@/views/authentication/TwoStep.vue"),
    props: () => ({ layout: LAYOUT_TYPES.COVER }),
    meta: {
      title: "Two Step",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/two-steps/boxed",
    name: "TwoStepBoxed",
    component: () => import("@/views/authentication/TwoStep.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BOXED }),
    meta: {
      title: "Two Step",
      authRequired: false,
      layout: BoxedLayout
    }
  },
  {
    path: "/two-steps/modern",
    name: "TwoStepModern",
    component: () => import("@/views/authentication/TwoStep.vue"),
    props: () => ({ layout: LAYOUT_TYPES.MODERN }),
    meta: {
      title: "Two Step",
      authRequired: false,
      layout: ModernLayout
    }
  },
  {
    path: "/logout/basic",
    name: "LogoutBasic",
    component: () => import("@/views/authentication/Logout.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Logout",
      authRequired: false,
      layout: BasicLayout
    }
  },
  {
    path: "/logout/cover",
    name: "LogoutCover",
    component: () => import("@/views/authentication/Logout.vue"),
    props: () => ({ layout: LAYOUT_TYPES.COVER }),
    meta: {
      title: "Logout",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/logout/boxed",
    name: "LogoutBoxed",
    component: () => import("@/views/authentication/Logout.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BOXED }),
    meta: {
      title: "Logout",
      authRequired: false,
      layout: BoxedLayout
    }
  },
  {
    path: "/logout/modern",
    name: "LogoutModern",
    component: () => import("@/views/authentication/Logout.vue"),
    props: () => ({ layout: LAYOUT_TYPES.MODERN }),
    meta: {
      title: "Logout",
      authRequired: false,
      layout: ModernLayout
    }
  },
  {
    path: "/reset-password/basic",
    name: "ResetPasswordBasic",
    component: () => import("@/views/authentication/ResetPassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Reset Password",
      authRequired: false,
      layout: BasicLayout
    }
  },
  {
    path: "/reset-password/cover",
    name: "ResetPasswordCover",
    component: () => import("@/views/authentication/ResetPassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.COVER }),
    meta: {
      title: "Reset Password",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/reset-password/boxed",
    name: "ResetPasswordBoxed",
    component: () => import("@/views/authentication/ResetPassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BOXED }),
    meta: {
      title: "Reset Password",
      authRequired: false,
      layout: BoxedLayout
    }
  },
  {
    path: "/reset-password/modern",
    name: "ResetPasswordModern",
    component: () => import("@/views/authentication/ResetPassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.MODERN }),
    meta: {
      title: "Reset Password",
      authRequired: false,
      layout: ModernLayout
    }
  },
  {
    path: "/create-password/basic",
    name: "CreatePasswordBasic",
    component: () => import("@/views/authentication/CreatePassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BASIC }),
    meta: {
      title: "Create Password",
      authRequired: false,
      layout: BasicLayout
    }
  },
  {
    path: "/create-password/cover",
    name: "CreatePasswordCover",
    component: () => import("@/views/authentication/CreatePassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.COVER }),
    meta: {
      title: "Create Password",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/create-password/boxed",
    name: "CreatePasswordBoxed",
    component: () => import("@/views/authentication/CreatePassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.BOXED }),
    meta: {
      title: "Create Password",
      authRequired: false,
      layout: BoxedLayout
    }
  },
  {
    path: "/create-password/modern",
    name: "CreatePasswordModern",
    component: () => import("@/views/authentication/CreatePassword.vue"),
    props: () => ({ layout: LAYOUT_TYPES.MODERN }),
    meta: {
      title: "Create Password",
      authRequired: false,
      layout: ModernLayout
    }
  }
];
const pagesRoutes = [
  {
    path: "/pages/coming-soon",
    name: "ComingSoon",
    component: () => import("@/views/pages/ComingSoon.vue"),
    meta: {
      title: "Coming Soon",
      authRequired: false,
      layout: CoverLayout
    }
  },
  {
    path: "/pages/maintenance",
    name: "Maintenance",
    component: () => import("@/views/pages/Maintenance.vue"),
    meta: {
      title: "Maintenance",
      authRequired: false,
      layout: CoverLayout
    }
  }
];
export const routes = [
  ...dashboardRoutes,
  ...authRoutes,
  ...pagesRoutes,
  ...accountRoutes
];
