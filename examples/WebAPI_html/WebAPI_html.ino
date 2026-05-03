#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PolyWiFiPortal.h>

#include "html.h"     // Webページを定義する、html[] PROGMEM として定義。

#define WiFi_SSID "YOUR_SSID"
#define WiFi_PASS "YOUR_PASSWORD"

AsyncWebServer server(80);
PolyWiFiPortal portal(server);

#define BUTTONLED 13

bool buttonOn = false;
float temperature = 23.4;  // センサーから読む

String processor(const String &var) {
  if (var == "COLOR") return buttonOn ? "#87CEFA" : "#FF6666";
  if (var == "STATE") return buttonOn ? "ON" : "OFF";
  return "";
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTONLED,OUTPUT);
  digitalWrite(BUTTONLED,buttonOn);

  portal.begin(WiFi_SSID, WiFi_PASS, 99);  // ← 最後は識別番号、出席番号
    // WiFi接続：IPアドレスはシリアル通信で表示、もしくは、 http://192.168.4.1/からリダイレクト
    // Web(SSID：ESP32-SETUP-番号/ パスワードは"12345678"）に接続、http://192.168.4.1 に接続
    // url: "/" は、IPアドレス表示に使用しているので使えないことに注意

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *r) {
    // html.hの文字配列 html[] にhtmlを登録し、index.htmlとして送信する
    r->send(200, "text/html", html, processor);  // processor()によりプレースホルダを置き換える
    Serial.println("called URL:" + r->url() + " , sended index.html.");
  });

  // スイッチ等の制御系APIの例
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *r) {
    buttonOn = !buttonOn;
    r->send(200);
    digitalWrite(BUTTONLED,buttonOn);
    Serial.println("called URL:" + r->url() + " , ON/OFFボタンが押されました。");
  });
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *r) {
    buttonOn = true;
    r->send(200);
    digitalWrite(BUTTONLED,buttonOn);
    Serial.println("called URL:" + r->url() + " , ボタンはONになりました。");
  });
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *r) {
    buttonOn = false;
    r->send(200);
    digitalWrite(BUTTONLED,buttonOn);
    Serial.println("called URL:" + r->url() + " , ボタンはOFFになりました。");
  });

  // 状態読み出し、json利用、sendChunkedの利用例（多重処理とメモリにやさしいがデータ量が小さいのでメリットない）
  server.on(
    "/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->sendChunked(
        "application/json",
        [request](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
          char json[128];
          int len = snprintf(
            json, sizeof(json),
            "{\"temp\":%.1f,\"button\":%s}",
            temperature,
            buttonOn ? "true" : "false");

          //if (index==0) Serial.println("called URL:" + request->url() + json);
          if (index >= (size_t)len) {  // 終了チェック
            Serial.println("called URL:" + request->url() + ",json=" + json);
            return 0;
          }

          size_t copyLen = min(maxLen, (size_t)len - index);
          memcpy(buffer, json + index, copyLen);
          return copyLen;
        });
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