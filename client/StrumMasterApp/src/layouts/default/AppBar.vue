<template>
  <v-app-bar class="px-3" color="white" flat density="compact" :elevation="5">
    <v-spacer></v-spacer>
    <v-tabs centered color="grey-darken-2" exact-active-class="active">
      <router-link to="/">
        <v-tab>Home</v-tab>
      </router-link>
      <router-link to="/play-song">
        <v-tab>Play song (via Midi)</v-tab>
      </router-link>
      <router-link to="/play-notes">
        <v-tab>Play notes</v-tab>
      </router-link>
      <router-link to="/debug">
        <v-tab>Debug and calibrate motors</v-tab>
      </router-link>
    </v-tabs>
    <v-spacer></v-spacer>
  </v-app-bar>
</template>

<script>
import { RouterLink } from 'vue-router'

export default {
  components: {
    RouterLink
  }
}
</script>

<style>
.router-link-exact-active .v-tab {
  color: inherit !important;
}

a {
  color: inherit;
  text-decoration: none;
}

a:hover {
  text-decoration: underline;
}
</style>