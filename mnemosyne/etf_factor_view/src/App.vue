<script setup lang="ts">
import HelloWorld from './components/HelloWorld.vue'
import AppHeader from './components/AppHeader.vue'
</script>

<template>

  <AppHeader />
  <HelloWorld msg="Hello Vue 3 + Vite" />
  <div class="container-fluid">
    <div class="card" style="width: 18rem;">
      <div class="card-body">
        <h5 class="card-title">Card title</h5>
        <p class="card-text">Some quick example text to build on the card title and make up the bulk of the card's content.</p>
        <a href="#" class="btn btn-primary">Go somewhere</a>
      </div>
    </div>
  </div>
  
</template>
