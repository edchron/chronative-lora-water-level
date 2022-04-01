void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(byte destination, String outgoing) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // destination
  LoRa.write(localAddress);             // localAddress
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                  // add payload
  LoRa.endPacket();                     // finish packet and send it
  LoRa_rxMode();                        // set rx mode
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;

  // read packet header bytes:
  byte recipient = LoRa.read();
  if (recipient != localAddress){
    Serial.println("This message is not for this Gateway.");
    return;
  }
    
  byte sender = LoRa.read();
  
  for (int i = 0; i < numNodes; i++){
    if (node[i].id == sender){
      long incomingMsgId = LoRa.read();
      boolean incomingType = LoRa.read();
      byte incomingLength = LoRa.read();
      String incoming = "";
      
      while (LoRa.available()) {
        incoming += (char)LoRa.read();
      }

      String device = node[i].nodeName;
      
      if (incomingLength != incoming.length()){
        Serial.println("Error: message length does not match");
        return;
      }
      
      strcpy(lidarTemp, incoming.c_str());
      sprintf(hexChar,"%02X",sender);

      if (incomingType){
        request = true;
      }
      else {
        normal = true;
      }
      
      Serial.print("Received from: 0x");
      Serial.print(hexChar);
      Serial.println(" ("+device+")");
      Serial.println("Message ID: " + String(incomingMsgId));
      Serial.print("Message Type: ");
      Serial.println((incomingType ? "Request" : "Normal"));
      Serial.println("Message Length: " + String(incomingLength));
      Serial.println("Message: " + incoming);
      Serial.print("RSSI: " + String(LoRa.packetRssi()));
      Serial.println(", SNR: " + String(LoRa.packetSnr()));
      Serial.println(); 
    }
  }  
}
