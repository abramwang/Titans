export default class LocalStorage {
  key: string;

  constructor(key: string) {
    this.key = key;
  }

  getItems() {
    const data = window.localStorage.getItem(this.key) || "{}";
    return JSON.parse(data);
  }

  setItems(value: any) {
    window.localStorage.setItem(this.key, JSON.stringify(value));
  }

  removeItem() {
    window.localStorage.removeItem(this.key);
  }
}
