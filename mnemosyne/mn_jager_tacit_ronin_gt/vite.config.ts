import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import path from "path";

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [vue()],
  resolve: {
    alias: {
      "@": path.resolve("./", "src")
    },
    extensions: [".js", ".json", ".jsx", ".mjs", ".ts", ".tsx", ".vue"]
  },
  server: {
    port: 3000
  },
  // build: {
  //   outDir: "/"
  // }
  // base: '/vue/tailwick/',
});
