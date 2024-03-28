import { createI18n, type I18nOptions } from "vue-i18n";
import localeMessages from "@/assets/lang";
import { App } from "vue";
import appConfigs from "@/app/appConfig.ts";

const legacy: boolean = false;
const globalInjection: boolean = true;
const locale: string = appConfigs.defaultLocale || "en";
const fallbackLocale: string =
  import.meta.env.VUE_APP_I18N_FALLBACK_LOCALE || "en";

export default createI18n({
  legacy,
  globalInjection,
  locale,
  fallbackLocale,
  messages: localeMessages,
  allowComposition: true
}) as { install: (app: App<I18nOptions>) => any };
