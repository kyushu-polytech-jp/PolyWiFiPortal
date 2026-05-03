#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PolyWiFiPortal.h>

#define WiFi_SSID "YOUR_SSID"
#define WiFi_PASS "YOUR_PASSWORD"

AsyncWebServer server(80);
PolyWiFiPortal portal(server);

void setup() {
  Serial.begin(115200);

  portal.begin(WiFi_SSID, WiFi_PASS, 99);
  // LocalIPが分からなければ、SoftAP SSID：ESP32-SETUP-1/12345678" に接続
  // WiFi接続：IPアドレスはシリアル通信で表示、もしくは、 http://192.168.4.1/からSTA側index.htmlへ切り替え

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    //request->send(200, "text/plain", "Hello, Web client");  // char[] は500バイト程度ならコピーして利用するため安全
    // String型は、スコープが終了すると送信できていなくとも解放されるから安全ではない。使わないこと。
    // ストリームを用意してそこに送信データを組み立てる（printはF()を用いたPROGMEM領域のリテラルが利用可能）
    AsyncResponseStream *res = request->beginResponseStream("text/html");
    res->print(F("<html><title>Hello Web</title><body>"));
    res->print(F("<h2>Hello Web API World by ESP32</h2>"));
    res->print(F("<p>Sample Web Page : http://"));
    res->print(WiFi.localIP());
    res->print(F("/index.html</p><p><a href=\"http://"));
    res->print(WiFi.localIP());
    res->print(F("/index.html\" target=\"_blank\"> this page </a></p><br>"));
    res->print(F("</body></html>"));
    request->send(res);
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