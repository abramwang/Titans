<script lang="ts" setup>
import Redis from "ioredis";
import { onMounted, ref } from "vue";

var con = null;
const message = ref('Hello World!')

onMounted(() => {
    con = new Redis({
        host: "121.36.210.85",
        port: 20184,
        password: "P7pO48Lw4AZTOLXKlR"
    });
});
</script>
<template>
    <PageHeader title="Ecommerce" :items="['Dashboards', 'Ecommerce']" />

    <div>
        
        <h1>{{ message }}</h1>
    </div>
</template>
