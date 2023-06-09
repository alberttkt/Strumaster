<template>
  <v-app id="inspire">
    <v-main class="bg-grey-lighten-3">
      <v-container>
        <v-row>
          <v-col cols="12" sm="3" :elevation="10">
            <v-sheet rounded="lg">
              <the-menu-bar />
            </v-sheet>
          </v-col>

          <v-col cols="12" sm="9">
            <v-sheet rounded="lg" :elevation="10">
              <v-container>

                <v-form ref="form">
                  <p class="text-center">Use functions to debug stepper<br><br></p>

                  <v-select v-model="stepperFunctionToUse"
                    :items="['PlayNote', 'Steps', 'Reset', 'Trigger', 'Reverse', 'Calibrate', 'CalibrateAll']"
                    label="Function to use" variant="outlined"></v-select>

                  <v-slider v-model="stepperId" :min="0" :max="5" :step="1" thumb-label label="Stepper ID"
                    @input="stepperId = $event"></v-slider>

                  <v-text-field v-model="stepperValue" label="Value" variant="outlined"></v-text-field>

                  <v-btn @click="debugStepper" block class="mt-2">Send to Stepper</v-btn><br><br>

                  <v-divider></v-divider><br><br>

                  <p class="text-center">Calibrate Steppers<br><br></p>

                  <v-row v-for="id in 6" :key="id">
                    <v-col>
                      <v-btn @click="trigger(id - 1)" prepend-icon="mdi-cursor-move" block variant="tonal" color="orange"
                        class="mt-2">
                        Trigger Stepper {{ id - 1 }}
                      </v-btn>
                    </v-col>
                    <v-col>
                      <v-btn @click="reverse(id - 1)" prepend-icon="mdi-keyboard-tab-reverse" block variant="tonal"
                        color="blue" class="mt-2">
                        Reverse Stepper {{ id - 1 }}
                      </v-btn>
                    </v-col>
                  </v-row><br><br>

                  <v-btn @click="calibrateAll" block prepend-icon="mdi-move-resize" variant="tonal" color="green"
                    class="mt-2">Calibrate all Steppers</v-btn><br><br>

                  <v-divider></v-divider><br><br>

                  <p class="text-center">Trigger servo<br><br></p>

                  <v-slider v-model="servoId" :min="0" :max="5" :step="1" thumb-label label="Servo ID"
                    @input="servoId = $event"></v-slider>

                  <v-btn @click="debugServo" block class="mt-2">Send to Servo</v-btn>

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
    stepperFunctionToUse: null,
    stepperId: 1,
    stepperValue: null,
    servoId: 1,
    snackbarVisible: false,
    snackbarText: '',
    snackbarColor: '',
    snackbarTimeout: 2000,
  }),
  methods: {
    debugStepper() {
      const url = `http://192.168.174.140/debug_stepper?function=${this.stepperFunctionToUse}&id=${this.stepperId}&value=${this.stepperValue}`
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
    debugServo() {
      const url = `http://192.168.174.140/debug_servo?id=${this.servoId}`
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
    trigger(id) {
      this.stepperId = id;
      this.stepperValue = null;
      this.stepperFunctionToUse = "Trigger";
      this.debugStepper();
      this.stepperFunctionToUse = "Steps";
      this.stepperValue = -10;
    },
    reverse(id) {
      this.stepperId = id;
      this.stepperValue = null;
      this.stepperFunctionToUse = "Reverse";
      this.debugStepper();
    },
    calibrateAll() {
      this.stepperId = 0;
      this.stepperValue = null;
      this.stepperFunctionToUse = "CalibrateAll";
      this.debugStepper();
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