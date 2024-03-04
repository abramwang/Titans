<script setup lang="ts">
import { ref } from 'vue'

defineProps<{ msg: string }>()

const count = ref(0)
</script>

<template>
  <div class="container-fluid">

    <h2>{{ msg }}</h2>
    <div class="card" style="width: 18rem;">
      <div class="card-body">
        <h5 class="card-title">Card title</h5>
        <table class="table">
          <thead>
            <tr>
              <th scope="col">#</th>
              <th scope="col">First</th>
              <th scope="col">Last</th>
              <th scope="col">Handle</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <th scope="row">1</th>
              <td>Mark</td>
              <td>Otto</td>
              <td>@mdo</td>
            </tr>
            <tr>
              <th scope="row">2</th>
              <td>Jacob</td>
              <td>Thornton</td>
              <td>@fat</td>
            </tr>
            <tr>
              <th scope="row">3</th>
              <td colspan="2">Larry the Bird</td>
              <td>@twitter</td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>


    <p class="read-the-docs">Click on the Vite and Vue logos to learn more</p>
    <div class="d-flex flex-row">
      <div class="p-2">Flex item 1</div>
      <div class="p-2">Flex item 2</div>
      <div class="p-2">Flex item 3</div>
    </div>
    <div class="d-flex flex-row-reverse">
      <div class="p-2">Flex item 1</div>
      <div class="p-2">Flex item 2</div>
      <div class="p-2">Flex item 3</div>
    </div>

    <div class="d-flex justify-content-start">
      <div class="p-2">Flex item 1</div>
      <div class="p-2">Flex item 2</div>
      <div class="p-2">Flex item 3</div>
    </div>
    <div class="d-flex justify-content-end">...</div>
    <div class="d-flex justify-content-center">...</div>
    <div class="d-flex justify-content-between">...</div>
    <div class="d-flex justify-content-around">
      <div class="p-2">Flex item 1</div>
      <div class="p-2">Flex item 2</div>
      <div class="p-2">Flex item 3</div>
    </div>

    <div class="fixed-bottom">fixed-bottom</div>
  </div>
  

</template>
