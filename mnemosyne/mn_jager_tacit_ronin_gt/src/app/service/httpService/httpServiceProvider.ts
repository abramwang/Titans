import FakeBackendService from "@/app/service/httpService/fakeBackendService.ts";
import { getFirebaseBackend } from "@/app/service/httpService/firebaseService.ts";
const fakeBackendService = new FakeBackendService();
const firebaseService = getFirebaseBackend();
export { fakeBackendService, firebaseService };
