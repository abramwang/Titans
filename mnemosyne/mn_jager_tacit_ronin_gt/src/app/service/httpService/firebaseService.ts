import firebase from "firebase/compat/app";
import "firebase/compat/auth";
import "firebase/compat/firestore";
import { UserType } from "@/app/service/httpService/types.ts";

class FirebaseAuthBackend {
  constructor(firebaseConfig: any) {
    if (firebaseConfig) {
      // Initialize Firebase
      firebase.initializeApp(firebaseConfig);
      firebase.auth().onAuthStateChanged((user) => {
        if (user) {
          sessionStorage.setItem("authUser", JSON.stringify(user));
        } else {
          sessionStorage.removeItem("authUser");
        }
      });
    }
  }

  /**
   * Registers the user with given details
   */
  registerUser(payload: UserType) {
    const { username, password, email = "" } = payload;

    return new Promise((resolve, reject) => {
      firebase
        .auth()
        .createUserWithEmailAndPassword(email, password)
        .then(
          () => {
            let user = firebase.auth().currentUser?.updateProfile({
              displayName: username
            });
            resolve(user);
          },
          (error) => {
            const message = this._handleError(error);
            reject(new Error(message));
          }
        );
    });
  }

  /**
   * Login user with given details
   */
  loginUser(payload: UserType) {
    const { password, email = "" } = payload;
    return new Promise((resolve, reject) => {
      firebase
        .auth()
        .signInWithEmailAndPassword(email, password)
        .then(
          () => {
            var user = firebase.auth().currentUser;
            sessionStorage.setItem("authUser", JSON.stringify(user));
            resolve(user);
          },
          (error) => {
            const message = this._handleError(error);
            reject(new Error(message));
          }
        );
    });
  }

  /**
   * forget Password user with given details
   */
  forgetPassword(payload: UserType) {
    const { email = "" } = payload;

    return new Promise((resolve, reject) => {
      firebase
        .auth()
        .sendPasswordResetEmail(email, {
          url: window.location.protocol + "//" + window.location.host + "/login"
        })
        .then(() => {
          resolve(true);
        })
        .catch((error) => {
          reject(this._handleError(error));
        });
    });
  }

  /**
   * Logout the user
   */
  logout() {
    return new Promise((resolve, reject) => {
      firebase
        .auth()
        .signOut()
        .then(() => {
          alert("logout");
          resolve(true);
        })
        .catch((error) => {
          reject(this._handleError(error));
        });
    });
  }

  setLoggedInUser(user: any) {
    sessionStorage.setItem("authUser", JSON.stringify(user));
  }

  /**
   * Returns the authenticated user
   */
  getAuthenticatedUser() {
    if (!sessionStorage.getItem("authUser")) return null;
    return JSON.parse(sessionStorage.getItem("authUser") || "");
  }

  /**
   * Handle the error
   * @param {*} error
   */
  _handleError(error: any) {
    var errorMessage = error.message;
    return errorMessage;
  }
}

let _fireBaseBackend: any = null;

/**
 * Initilize the backend
 * @param {*} config
 */
const initFirebaseBackend = (config: any) => {
  if (!_fireBaseBackend) {
    _fireBaseBackend = new FirebaseAuthBackend(config);
  }
  return _fireBaseBackend;
};

/**
 * Returns the firebase backend
 */
const getFirebaseBackend = () => {
  return _fireBaseBackend;
};

export { initFirebaseBackend, getFirebaseBackend };
