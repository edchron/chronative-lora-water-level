/*******************************************************
   LoRa Gate v1 from Chronative
   MCU: ESP32
   Sensors: LiDAR
 *******************************************************/

// LoRa libraries
#include <LoRa.h>
#include <SPI.h>

// MQTT & WiFi libraries
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ========== CHANGE ONLY BELOW PARAMETERS ================== //

// MQTT Broker RPi3
#define mqttBroker "192.168.131.33"
#define mqttPort 1883   // Default, No Need to change

// SET LORA FREQUENCY
const long frequency = 868E6;   // Default, can only change to 868E6

// SET LORA PINS
const int csPin = 5;          // LoRa radio chip select
const int resetPin = 14;      // LoRa radio reset
const int irqPin = 13;         // change for your board; must be a hardware interrupt pin

// SET LORA SPREADING FACTOR
int setSF = 9;

// SET LORA BANDWIDTH
int setBW = 125E3;

// SET LORA SYNCWORD, Defualt 0x34 or 0x12
int setSync = 0x34;

// SET LORA TX POWER
int setTxPwr = 20;

// VARIABLES FOR LORA
byte localAddress = 0xFF;
static char hexChar[3];
boolean request;
boolean normal;

// VARIABLES FOR MQTTT
static char lidarTemp[7];

// -------------- LIST OF WIFI ---------------------- //
struct wpas {
  char ssid[32];             // Maximum Buffer Size (and allocated memory)
  char pass[64];
};

wpas wpa[] = {
  { "Galaxy A51", "galaxyedwin" },
  { "dd-wrt" , "" },
  {"Wifi KKBJ 2", "pdnomer19"}
};

int numWpas = sizeof(wpa) / sizeof(wpa[0]);

// -------------- LIST OF NODE ---------------------- //
struct nodex {
  byte id;              // This is the LoRa ID
  char nodeName[32];    // Name of the node
};

nodex node[] {
  { 0x00, "Pool 1"},
  { 0x01, "Pool 2"},
  { 0x02, "Pool 3"}
};

int numNodes = sizeof(node) / sizeof(node[0]);

// -------------- LIST OF PUBLISH TOPICS ------------- //
const char* pool1_lidar = "chronative/pool1/lidar";
const char* pool2_lidar = "chronative/pool2/lidar";


// -------------- LIST OF SUBSCRIBE TOPICS ----------- //
const char* pool1_req = "chronative/pool1/req";
const char* pool2_req = "chronative/pool2/req";


// -------- LIST OF REQUEST CALLBACK TOPICS ---------- //
const char* pool1_reqCallback = "chronative/pool1/reqCallback";
const char* pool2_reqCallback = "chronative/pool2/reqCallback";


// ================= END OF PARAMETERS ==================== //

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);

  delay(20);
  Serial.println("Initializing...");

  // Wifi init
  Serial.println("WiFi init...");
  wifiSetup();

  // MQTT init
  Serial.println("MQTT server and callback init...");
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(callback);

  // LoRa init
  Serial.println("LoRa init...");

  // setup LoRa transceiver module
  LoRa.setPins(csPin, resetPin, irqPin);

  while (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    Serial.println("Trying again in 5 seconds");
    delay(5000);
  }
  Serial.println("LoRa is connected");

  // Set SF and BW for Data Rate default SF9BW125K
  LoRa.setSpreadingFactor(setSF);
  LoRa.setSignalBandwidth(setBW);
  LoRa.setSyncWord(setSync);
  LoRa.setTxPower(setTxPwr);

  LoRa.onReceive(onReceive);
  LoRa_rxMode();
}

void loop() {

  if (WiFi.status() != WL_CONNECTED || !mqttClient.connected()) {
    Serial.print("Trying to reconnect to WiFi");
    wifiSetup();
    if (!mqttClient.connected()) {
      reconnect();
    }
  }
  mqttClient.loop();

  if (normal) {
    publishData(pool1_lidar);
    normal = false;
  }


  if (request) {
    publishData(pool1_reqCallback);
    request = false;
  }

}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
