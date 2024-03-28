const {
  VITE_APP_I18N_LOCALE,
  VITE_APP_DEFAULT_AUTH,
  VITE_APP_APIKEY,
  VITE_APP_AUTHDOMAIN,
  VITE_APP_DATABASEURL,
  VITE_APP_PROJECTID,
  VITE_APP_STORAGEBUCKET,
  VITE_APP_MESSAGINGSENDERID,
  VITE_APP_APPID,
  VITE_APP_MEASUREMENTID
} = import.meta.env;

const appConfigs = {
  auth: VITE_APP_DEFAULT_AUTH,
  // baseUrl: VITE_BASE_URL,
  defaultLocale: VITE_APP_I18N_LOCALE,
  googleMapAPIKey: "AIzaSyAbvyBxmMbFhrzP9Z8moyYr6dCr-pzjhBE",

  // firebase keys
  fireBaseApiKey: VITE_APP_APIKEY,
  fireBaseAuthDomain: VITE_APP_AUTHDOMAIN,
  fireBaseDBUrl: VITE_APP_DATABASEURL,
  fireBaseProjectId: VITE_APP_PROJECTID,
  fireBaseStorageBucket: VITE_APP_STORAGEBUCKET,
  fireBaseMsgSenderId: VITE_APP_MESSAGINGSENDERID,
  fireBaseAppId: VITE_APP_APPID,
  fireBaseMeasurementId: VITE_APP_MEASUREMENTID
};

export default appConfigs;
