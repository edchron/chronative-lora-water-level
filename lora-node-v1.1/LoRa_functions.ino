void LoRa_rxMode() {
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode() {
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String outgoing, boolean msgType) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(msgType);                  // add message type (0: normal or 1: request);
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  LoRa_rxMode();                        // set rx mode
  msgCount++;
}

void onReceive(int packetSize) {
  if (packetSize == 0 ) return;   // If there is no packet

  // Read packet header bytes
  byte recipient = LoRa.read();
  byte sender = LoRa.read();

  if (recipient != localAddress && sender != destination) {
    Serial.println("This message not for this node.");
    return;
  }

  byte incomingLength = LoRa.read();
  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {
    Serial.println("Error: message length does not match");
    return;
  }

  sprintf(hexChar, "%02X", sender);

  Serial.print("Received from: 0x");
  Serial.print(hexChar);
  Serial.println(" (Gateway)");
  Serial.println("Message Length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.print("RSSI: " + String(LoRa.packetRssi()));
  Serial.println(", SNR: " + String(LoRa.packetSnr()));
  Serial.println();

  if (incoming == "get") {
    Serial.println("get received");
    request = true;
  }
}
