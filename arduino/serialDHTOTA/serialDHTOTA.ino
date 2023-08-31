// Libraries
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "DHT.h"
#include "secrets.h"

// Pin labeling
#define LED1 2
#define DHTPIN 13
#define DHTTYPE DHT11

// Constants
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

// Variables
unsigned long tblink = 0;
bool ledstate = false;
float t = 0;
float h = 0;
DHT dht(DHTPIN, DHTTYPE);
TaskHandle_t Task1;

// Subroutines and functions
void wifiInit() {
  Serial.println("[WiFi] WiFi Init");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("[WiFi] Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.print("[WiFi] IP address: ");
  Serial.println(WiFi.localIP());
}

void otaInit() {
  Serial.println("[OTA] OTA Init");
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("dvelas25_esp32");

  // No authentication by default
  ArduinoOTA.setPassword(SECRET_OTA_PASS);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("[OTA] Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\n[OTA] End");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("[OTA] Progress: %u%%\r\n", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("[OTA] Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  Serial.println("[OTA] OTA Ready");
}

void multiCoreInit() {
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */
  delay(500);
}

void readDHT() {
  float tact = dht.readTemperature();
  float hact = dht.readHumidity();

  if (isnan(tact) || isnan(hact)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  t = tact;
  h = hact;
}

void setup() {
  // Pin configuration
  pinMode(LED1, OUTPUT);

  // Digital Output Clean
  digitalWrite(LED1, LOW);

  // Comms
  Serial.begin(115200);
  Serial.println("Booting");
  dht.begin();
  wifiInit();
  otaInit();
  multiCoreInit();
  tblink = millis();
  Serial.print("[CPU] Main running on core ");
  Serial.println(xPortGetCoreID());
}

//Task1code: OTA task
void Task1code(void* pvParameters) {
  Serial.print("[CPU] Task1 running on core ");
  Serial.println(xPortGetCoreID());

  while(1) {
    ArduinoOTA.handle();
    vTaskDelay(10);
  }
}

void loop() {
  if (millis() - tblink >= 500) {
    ledstate = !ledstate;
    digitalWrite(LED1, ledstate);
    tblink = millis();
  }
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    readDHT();
    switch (command[0]) {
      case 't':
        Serial.print("t: ");
        Serial.println(t, 2);
        break;
      case 'h':
        Serial.print("h: ");
        Serial.println(h, 2);
        break;
      case 'a':
        Serial.print("t: ");
        Serial.print(t, 2);
        Serial.print(", h: ");
        Serial.println(h, 2);
        break;
      default:
        Serial.println("Invalid command");
    }
  }
}