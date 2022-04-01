// WiFi Settings

void wifiSetup() {
  delay(10);
  for (int i = 0; i < numWpas; i++) {
    // We start by connecting to a WiFi network
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi connected");
      Serial.print("ESP IP address: ");
      Serial.println(WiFi.localIP());
      return;
    }

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wpa[i].ssid);
    WiFi.begin(wpa[i].ssid, wpa[i].pass);
    while (WiFi.status() != WL_CONNECTED) {
      if (runEvery(10000)) {
        Serial.println();
        Serial.println("WiFi connection took too long, changing to the next AP");
        if (i == numWpas - 1) {
          Serial.println("Trying from the first AP again");
          i = -1;
        }
        break;
      }
      else {
        Serial.print(".");
        delay(500);
      }
    }
  }
}
