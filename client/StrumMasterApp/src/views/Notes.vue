<template>
  <v-app id="inspire">
    <v-main class="bg-grey-lighten-3">
      <v-container>
        <v-row>
          <v-col cols="12" sm="3">
            <v-sheet rounded="lg" :elevation="10">
              <the-menu-bar />
            </v-sheet>
          </v-col>

          <v-col cols="12" sm="9">
            <v-sheet rounded="lg" :elevation="10">
              <v-container>

                <v-form ref="form">
                  <p class="text-center">Enter the note you want to play<br><br></p>

                  <v-text-field v-model="note" label="Note" variant="outlined"></v-text-field>

                  <v-btn @click="playNote" block class="mt-2">Play note</v-btn><br><br>

                  <v-divider></v-divider><br><br>

                  <p class="text-center">Or click the note you want to play<br><br></p>

                  <v-row v-for="fret_id in 9" :key="fret_id">
                    <v-col v-for="stepper_id in 6" :key="stepper_id">
                      <v-btn @click="clickNote('' + (stepper_id - 1) + (fret_id - 1))" block variant="tonal" class="mt-2">
                        Play id {{ '' + (stepper_id - 1) + (fret_id - 1) }}
                      </v-btn>
                    </v-col>
                  </v-row>

                </v-form>
              </v-container>
            </v-sheet>
          </v-col>
        </v-row>
      </v-container>
    </v-main>
    <v-snackbar v-model="snackbarVisible" :color="snackbarColor" :timeout="snackbarTimeout">{{ snackbarText
    }}</v-snackbar>
  </v-app>
</template>

<script>
import TheMenuBar from '../components/Menu.vue'

export default {
  components: {
    TheMenuBar,
  },
  data: () => ({
    note: null,
    snackbarVisible: false,
    snackbarText: '',
    snackbarColor: '',
    snackbarTimeout: 2000,
  }),
  methods: {
    playNote() {
      const url = `http://192.168.174.140/play_note?id=${this.note}`
      console.log("Sending GET request to: " + url);
      fetch(url)
        .then(response => {
          if (response.ok) {
            return response.text();
          } else {
            this.showSnackbar('An error occurred', 'warning')
          }
        })
        .then(response => {
          if (response) {
            this.showSnackbar(response, 'success');
          }
        })
        .catch(error => {
          console.error(error)
          this.showSnackbar('We lost connection with the board', 'error')
        })
    },
    clickNote(id) {

      this.note = id
      this.playNote()
    },
    showSnackbar(text, color) {
      this.snackbarText = text
      this.snackbarColor = color
      this.snackbarVisible = true
      console.log("Snackbar showed: ", text)
    },
  },
}
</script>
