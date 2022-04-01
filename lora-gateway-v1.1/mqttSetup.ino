// Reconnect setup
void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Client
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");

      // Publish after connected
      mqttClient.publish("chronative/connect", "Gateway is connected");

      // LIST OF SUBSCRIBED TOPICS
      mqttClient.subscribe(pool1_req, 1);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Callback setup
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // SUBSCRIBED TOPICS (If necessary)
  if (topic == pool1_req) {
//    Serial.println(messageTemp);
    Serial.println("Sending Message to: 0x00 (Pool 1)");
//    Serial.println(node[0].id, HEX);
    LoRa_sendMessage(node[0].id, messageTemp); // send a message
    Serial.println("Message sent!");
    Serial.println();
  }
}
