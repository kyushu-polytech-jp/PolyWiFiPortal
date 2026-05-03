#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PolyWiFiPortal.h>

#include "html.h"  // Webページを定義する、html[] PROGMEM として定義。

#define WiFi_SSID "YOUR_SSID"
#define WiFi_PASS "YOUR_PASSWORD"

AsyncWebServer server(80);
PolyWiFiPortal portal(server);

// htmlの%LOCALIP%プレースホルダを置き換えるメソッド
String processor(const String &var) {
  if (var == "LOCALIP") return WiFi.localIP().toString();
  return "";
}

void setup() {
  Serial.begin(115200);

  portal.begin(WiFi_SSID, WiFi_PASS, 99);
  // LocalIPが分からなければ、SoftAP SSID：ESP32-SETUP-1/12345678" に接続
  // WiFi接続：IPアドレスはシリアル通信で表示、もしくは、 http://192.168.4.1/からSTA側index.htmlへ切り替え

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", html, processor);
    Serial.println("called URL:" + request->url() + " , sended index.html.");
  });

  // 宛先不明のパス要求があったらここで返事する
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
    Serial.println("called URL:" + request->url() + " , sended 404 Not found.");
  });

  server.begin();
}

void loop() {
  // Asyncなので空でOK
}