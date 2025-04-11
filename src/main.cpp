/****************************************************************************************************************************
  *** Modbus-TCP Server und Firmware upload über RJ45 ***
  
  Modbus-TCP-Server:
    - Mit den Konstanten HR_START, HR_COUNT kann der Bereich für Holdingregister bestimmt werden.
    - Die Register werden dann mit Werten gefüllt die der Adresse entsprechen. 
    - Weitere Register für Inputregister oder Coils, sind aktuell ausommentiert.

  Für Firmware upload über Ethernet (OTA) :
    - Projekt in VSC / PlatformIO kompilieren
    - Im Projektordner unter "pio/build/ESP32-ETH01" wird die "firmware.bin" erzeugt
    - Im Browser zu http://192.168.X.X/update segeln 
    - "Datei Auswählen" anklicken und die "firmware".bin" Datei die eben erstellt wurde auswählen.
    - Danach muss einmal die Speisung getrennt werden?
    */

// Version v1.1  Mit Zähler 100ms Register 1000  /  1 sek. Register 1001
// Version v1.2  mit main.h / main.cpp. Ohne Serial Debug Output

#include <main.h>

void setup(){
  Serial.begin(115200);

  WT32_ETH01_onEvent();
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);
  ETH.config(IP, GW, SN, DNS);    // Static IP, leave without this line to get IP via DHCP

  WT32_ETH01_waitForConnect();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(200, "text/plain",  "Hi! For Firmware Update over OTA go to /update ");
  });

  AsyncElegantOTA.begin(&server);   // Start ElegantOTA
  server.begin();                   // Start Server für Web
  mbServer.begin();                 // Start Server für MB

  // start the Modbus TCP server
  if (!mb.begin()) {
    while (1);
  }

  // Inputregister konfigurieren
  //mb.configureInputRegisters(0, 100);
  // Coils konfigurieren
  //mb.configureCoils(0, 100);

  // Holdingregister konfigurieren
  mb.configureHoldingRegisters(HR_START, HR_COUNT);
  for (size_t i = HR_START; i < HR_COUNT-2; i++){
    mb.holdingRegisterWrite(i,i);  // Füllen der Register mit Wert=Register Nr.
  }
}


void loop() {
  pixelWrite();
  WiFiClient client = mbServer.available();     // listen for incoming clients
  if (client) {
    mb.accept(client);       // let the Modbus TCP accept the connection
    while (client.connected()) {
      mb.poll();         // poll for Modbus TCP requests, while client connected
      pixelWrite();
      writeCounter();
    }
  }
}
  
void writeCounter(){
  unsigned long actms = millis();
  if (actms >= last_timer100ms + 100){
    last_timer100ms = actms;
    mb.holdingRegisterWrite(HR_COUNT-2,counter_100ms);
    counter_100ms ++;
  }
  if (actms >= last_timer1000ms + 1000){
    last_timer1000ms = actms;
    mb.holdingRegisterWrite(HR_COUNT-1,counter_1000ms);
    counter_1000ms ++;
  }
}

void pixelWrite(){
  u8_t led1_red = constrain(mb.holdingRegisterRead(0), 0, 255);
  u8_t led1_green = constrain(mb.holdingRegisterRead(1), 0, 255);
  u8_t led1_blue = constrain(mb.holdingRegisterRead(2), 0, 255);
  u8_t led2_red = constrain(mb.holdingRegisterRead(3), 0, 255);
  u8_t led2_green = constrain(mb.holdingRegisterRead(4), 0, 255);
  u8_t led2_blue = constrain(mb.holdingRegisterRead(5), 0, 255);
  u8_t led3_red = constrain(mb.holdingRegisterRead(6), 0, 255);
  u8_t led3_green = constrain(mb.holdingRegisterRead(7), 0, 255);
  u8_t led3_blue = constrain(mb.holdingRegisterRead(8), 0, 255);

  pixels.setPixelColor(0, pixels.Color(led1_red, led1_green, led1_blue));
  pixels.setPixelColor(1, pixels.Color(led2_red, led2_green, led2_blue));
  pixels.setPixelColor(2, pixels.Color(led3_red, led3_green, led3_blue));
  pixels.show();
}c
