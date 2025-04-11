#include <Arduino.h>
#include <WebServer_WT32_ETH01.h>         // https://github.com/khoih-prog/WebServer_WT32_ETH01/
#include <AsyncTCP.h>                     // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>            // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncElegantOTA.h>              // https://github.com/ayushsharma82/AsyncElegantOTA
#include <ArduinoRS485.h>                 // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>
#include <Adafruit_NeoPixel.h>

#define DEBUG_ETHERNET_WEBSERVER_PORT       Serial

const int HR_START = 0;
const int HR_COUNT = 1002;
unsigned long last_timer100ms;
unsigned long last_timer1000ms;
int16_t counter_100ms;
int16_t counter_1000ms;

// Set according to your local network if you need static IP
IPAddress IP(192, 168, 1, 216);
IPAddress GW(192, 168, 1, 1);
IPAddress SN(255, 255, 255, 0);
IPAddress DNS(8, 8, 8, 8);

AsyncWebServer server(80);
WiFiServer mbServer(502);
ModbusTCPServer mb;
Adafruit_NeoPixel pixels(3, 2, NEO_GRB + NEO_KHZ800);  // Anz. Pixel, Pin

void pixelWrite();
void writeCounter();