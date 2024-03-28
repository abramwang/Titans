import { createRouter, createWebHistory } from "vue-router";
import { routes } from "@/router/routes.ts";
import appConfigs from "@/app/appConfig.ts";
import { fakeBackendService } from "@/app/service/httpService/httpServiceProvider.ts";

const router = createRouter({
  history: createWebHistory(),
  routes,
});

const title = "Tailwick Vue - Admin & Dashboard Template";

router.beforeEach((to: any, from: any, next: any) => {
  window.scrollTo({
    top: 0,
    behavior: "smooth"
  });
  from; // NOTE: on build time it is giving error so just put it here
  const nearestWithTitle = to.matched
    .slice()
    .reverse()
    .find((r: any) => r.meta && r.meta.title);

  if (nearestWithTitle) {
    document.title = nearestWithTitle.meta.title + " | " + title || title;
  }

  const isAuthRequired = to.meta.authRequired;

  if (!isAuthRequired) {
    return next();
  } else {
    const auth = appConfigs.auth;
    if (auth === "fakebackend") {
      const user = fakeBackendService.getUser();
      const isUserLoggedIn = Object.keys(user).length > 0;
      if (isAuthRequired && isUserLoggedIn) {
        next();
      } else {
        router.push("/login");
      }
    } else {
      next();
    }
  }
});

export default router;
