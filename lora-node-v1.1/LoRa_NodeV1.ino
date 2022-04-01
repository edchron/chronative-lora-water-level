/*******************************************************
   LoRa Node v1 from Chronative
   MCU: ESP32
   Sensors: LiDAR
 *******************************************************/
 
// LoRa libraries
#include <SPI.h>
#include <LoRa.h>

// TFMPlus libraries
//#include <TFMini.h>
//TFMini tfmini;
#include <TFMPlus.h>
TFMPlus tfmp;

// ========== CHANGE ONLY BELOW PARAMETERS ================== //
# define TFM_SERIAL Serial2

// SET LORA FREQUENCY
const long frequency = 868E6;  // LoRa Frequency

// SET LORA PINS
const int csPin = 5;          // LoRa radio chip select
const int resetPin = 14;        // LoRa radio reset
const int irqPin = 13;          // change for your board; must be a hardware interrupt pin

// SET LORA SPREADING FACTOR
int setSF = 9;

// SET LORA BANDWIDTH
int setBW = 125E3;

// SET LORA SYNCWORD, Defualt 0x34 or 0x12
int setSync = 0x34;

// SET LORA TX POWER
int setTxPwr = 20;

// VARIABLES FOR LORA
unsigned short msgCount = 0;  // count of outgoing messages
byte localAddress = 0x00;     // address of this device
byte destination = 0xFF;      // destination to send to
static char hexChar[3];
boolean request;
unsigned long toMinute = 60000; // 60000 milliseconds in a minute
unsigned long publishNormal = 20; // Change publish time interval for LoRa (in minutes)

// VARIABLES FOR TFMINI
unsigned long  previousMillis = 0;  // access time
const long interval = 25;           // interval at which to measure (milliseconds)

String lidar;

// ================= END OF PARAMETERS ==================== //

void setup() {
  Serial.begin(115200);                   // initialize serial
  delay(20);
  while (!Serial);
  Serial.println("Initializing...");

  // TF Mini Plus init
  Serial.println("TFMini init...");

  TFM_SERIAL.begin(115200);
  delay(20);
  tfmp.begin(&TFM_SERIAL);

  Serial.println("TFMini initialized");

  Serial.println("LoRa init...");
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
  Serial.println("LoRa initialized");
  Serial.println("System is ready!");
}

void loop() {
  getLidar();
  //  Serial.println(lidar);
  if (runEvery(publishNormal * toMinute)) { // repeat every 1000 millis
    String message = lidar;

    Serial.print("Sending Message: " + message);
    LoRa_sendMessage(message, 0); // send a message
    Serial.println(" -> message sent!");
  }

  if (request) {
    String message = lidar;

    Serial.print("Sending Request: " + message);
    LoRa_sendMessage(message, 1); // send a message
    Serial.println(" -> message sent!");
    request = false;
  }
}

void getLidar() {
  unsigned long currentMillis = millis();
  int16_t dist;
  if (currentMillis - previousMillis >= interval) {

    // save the last measure time
    previousMillis = currentMillis;

    // Take one TF Mini distance measurement
    if (tfmp.getData(dist)) {
      lidar = String(dist);
    }
    //    dist = tfmini.getDistance();
  }
  //  lidar = String(dist);
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
