// Publish data to mqtt

void publishData(const char* topic) {
    boolean rc0 = mqttClient.publish(topic, lidarTemp);
    Serial.print("Pubslih to: ");
    Serial.println(topic);
    Serial.print((rc0 ? "Distance_to_Water:" : "FAILED: "));
    Serial.print(lidarTemp);
    Serial.println(" cm");
    Serial.println("==============================");
    // Serial: toWater:l cm
}
