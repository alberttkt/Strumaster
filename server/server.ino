#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "servo_library.h"

// Webserver config
const char* ssid = "strumaster_control";
const char* password = "12345678";
IPAddress local_ip(192, 168, 174, 140);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server(80);


// Program variables
bool connected = 0;
bool is_playing = false;
uint8_t nbFrets = 10;  // number of frets handled by strings

// CNC shield handling
#define DEFAULT_CNC_TIME_LET 3000
int cnc_pin = D4;
unsigned long cnc_timer = 0;

void setup() {
  Serial.begin(9600);

  // Servos init
  setupServos(10, 11, 12, 13, 14, 15);

  //cnc shield init
  pinMode(cnc_pin, OUTPUT);

  // WiFi initialisation
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  // Commands handle
  server.on("/connect", handleConnect);
  server.on("/stop", handleStop);
  server.on("/pause", handlePause);
  server.on("/resume", handleResume);
  server.on("/play_song", handlePlay);
  server.on("/reset_all", handleReset); 
  server.onNotFound(handleNotSupported);

  server.on("/load_song", HTTP_POST, []() {  // arg: POST_plain
    handleLoad();
  });
  server.on("/load_song", HTTP_OPTIONS, handleCORS);

  server.on("/play_note", HTTP_GET, []() {  // arg: id
    handlePlayNote();
  });

  server.on("/debug_stepper", HTTP_GET, []() {  // arg: function, id, value
    handleDebugStepper();
  });

  server.on("/debug_servo", HTTP_GET, []() {  // arg: id
    handleDebugServo();
  });

  server.enableCORS(true);
  server.begin();  
  
}
void loop() {
  // server
  server.handleClient();
  
  // Playing a song
  if (is_playing) loopTiming();

  // Cnc handling
  if (is_playing || millis() < cnc_timer) {
    digitalWrite(cnc_pin, LOW);
  } else {
    digitalWrite(cnc_pin, HIGH);
  }
}

//----------------- debug functions

void activate_stepper(int id_note) {
  cnc_timer = millis() + DEFAULT_CNC_TIME_LET;
  Serial.printf("%d,\n", id_note);
}

void debug_stepper(int id_stepper, int steps) {
  cnc_timer = millis() + DEFAULT_CNC_TIME_LET;
  Serial.printf("%d,%d,\n", id_stepper, steps);
}

void trigger_stepper(int id_stepper) {
  cnc_timer = millis() + 1000; // one second is enough
  debug_stepper(id_stepper, 10);
  delay(100);
  debug_stepper(id_stepper, -10);
}

void reverse_stepper(int id_stepper) {
  Serial.printf("%d,%d,\n", -1, id_stepper);
}

void reset_stepper(int id_stepper) {
  cnc_timer = millis() + DEFAULT_CNC_TIME_LET;
  Serial.printf("%d,%d,\n", -4, id_stepper);
}

void calibrate_stepper(int id_stepper){
  Serial.printf("%d,%d,\n", -2, id_stepper);
}

void activate_servo(int id_servo) {
  playSingleCord(id_servo);
}



//----------------- request handling functions 

void handleConnect() {
  server.send(200, "text/plain", "Connexion successful. Try to play a note !");
}

void handlePause() {
  is_playing = false;
  server.send(200, "text/plain", "Song paused");
}

void handleResume() {
  is_playing = true;
  server.send(200, "text/plain", "Song resumed");
}

void handlePlay() {
  is_playing = true;
  startPlaying();
  server.send(200, "text/plain", "Song started ! Enjoy !");
}

void handleStop() {
  is_playing = false;
  resetQueues();
  server.send(200, "text/plain", "All motor off");
}

void handleReset() {
  cnc_timer = millis() + DEFAULT_CNC_TIME_LET;
  Serial.println("-5,");
  server.send(200, "text/plain", "Device reinitialized");
}

void handleNotSupported() {
  server.send(501, "text/plain", "Command not supported");
}

void handleLoad() {
    cnc_timer = millis() + DEFAULT_CNC_TIME_LET;
    setupTimings(server.arg("plain").c_str());
    is_playing = false;
    server.send(200, "text/plain", "Song was sent to the guitar. File was correctly loaded !");
}

void handlePlayNote() {
  int id = server.arg("id").toInt();
  digitalWrite(cnc_pin, LOW);
  activate_stepper(id);
  delay(DEFAULT_CNC_TIME_LET);
  activate_servo(id / nbFrets);

  server.send(200, "text/plain", "Note sent and played");
}

void handleDebugStepper() {
  int id = server.arg("id").toInt();
  int value = server.arg("value").toInt();
  String function = server.arg("function");
  int rtnv = 0;

  if        (function == "PlayNote") {
    rtnv = 1;
  } else if (function == "Steps") {
    rtnv = 2;
  } else if (function == "Reset") {
    rtnv = 3;
  } else if (function == "Reverse") {
    rtnv = 4;
  } else if (function == "Trigger") {
    rtnv = 5; //todo
  } else if (function == "Calibrate"){
    rtnv = 6; //todo
  } else if (function == "CalibrateAll"){
    rtnv = 7; //todo
  } else {
    server.send(501, "text/plain", "Unknown debug function !");
    return;
  }

  switch (rtnv) {
    // Make a stepper go to a certain note
    case 1:
      activate_stepper(value);
      break;

    // Move the stepper <value> steps
    case 2:
      debug_stepper(id, value);
      break;

    // Reset the stepper's position
    case 3:
      reset_stepper(id);
      break;

    // Reverse the stepper
    case 4:
      reverse_stepper(id);
      break;

    // Trigger
    case 5:
      trigger_stepper(id);
      break;
    
    // calibrate
    case 6:
      calibrate_stepper(id);
      break;

    // calibrate all
    case 7:
      Serial.println("-3");
      break;

    default:
      return;
  }

  server.send(200, "text/plain", "Debug function received by ESP !");
}

void handleDebugServo() {
  int id = server.arg("id").toInt();

  //playSingleCord(id);
  activate_servo(id);

  server.send(200, "text/plain", "Servo moved.");
}

void handleCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}
