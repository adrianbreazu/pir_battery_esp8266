// read more https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DEBUG true

extern "C" {
  #include "gpio.h"
  #include "user_interface.h"
}

// WiFi connection details
const char* SSID = "__ssid__";
const char* PASSWORD = "__password__";
const char* URL = "http://__url_here__";
const int WIFI_CONNECT_COUNT = 50;

bool useWiFi = true;

IPAddress IP(192,168,1,20);
IPAddress GATEWAY(192,168,1,1);
IPAddress SUBNET(255,255,255,0);
// no DNS needed for intranet, add DNS for www
IPAddress DNS(0,0,0,0);

// -------------------- init WiFi --------------------
void connect_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.config(IP,GATEWAY,SUBNET);
  WiFi.begin(SSID, PASSWORD);
  int wifi_count = 0;
  while(WiFi.status() != WL_CONNECTED && wifi_count < WIFI_CONNECT_COUNT) {
    delay(500);
    wifi_count++;
    if (DEBUG)
      Serial.print(".");
  }
  if (wifi_count < WIFI_CONNECT_COUNT)
    useWiFi = true;
  else
    useWiFi = false;
  
  if (DEBUG) {
    Serial.println("");
    Serial.print("WiFi connected to ");
    Serial.println(SSID);
  }
}

void sendPOSTRequest() {
  String JSON_data = "";

  if (WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    http.begin(URL);
    http.addHeader("Content-Type", "application/json");

    JSON_data = "{ \"detection\": \"TRUE\" }";

    int httpCode = http.POST(JSON_data);
    String response = http.getString();

    if (DEBUG) {
      Serial.println("Request data");
      Serial.println(JSON_data);
      
      Serial.print("post httpCode: ");
      Serial.println(httpCode);
      
      Serial.println("Response data");
      Serial.println(response);
    }

    http.end();
    delay(500);
    if (DEBUG)
      Serial.println("close connection");
  }else{
    if (DEBUG)
      Serial.println("error in WiFi connection");
  }
}

void setup() {
  Serial.begin(115200);
  if (DEBUG) {
    Serial.println();
    Serial.println("in setup");
  }
  delay(300);
  
  connect_wifi();
  
  if (useWiFi) {
    sendPOSTRequest();
    if (DEBUG)
      Serial.println("Motion detected");
  }
  else {
    if (DEBUG)
      Serial.println("No WiFi, go back to sleep");
  }
    
  delay(500);
  ESP.deepSleep(0);
}

void loop() {
  if (DEBUG)
    Serial.println("in loop");
}
