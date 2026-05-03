#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PolyWiFiPortal.h>

#define WiFi_SSID "YOUR_SSID"
#define WiFi_PASS "YOUR_PASSWORD"

AsyncWebServer server(80);
PolyWiFiPortal portal(server);

#define BUTTONLED 13
bool buttonOn = false;
float temperature = 23.4;  // センサーから読む

void setup() {
  Serial.begin(115200);

  pinMode(BUTTONLED,OUTPUT);
  digitalWrite(BUTTONLED,buttonOn);

  portal.begin(WiFi_SSID, WiFi_PASS, 99);  // ← 最後は識別番号、出席番号
    // WiFi接続：IPアドレスはシリアル通信で表示、もしくは、 http://192.168.4.1/からリダイレクト
    // Web(SSID：ESP32-SETUP-番号/ パスワードは"12345678"）に接続、http://192.168.4.1 に接続
    // url: "/" は、IPアドレス表示に使用しているので使えないことに注意

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *r) {
    AsyncResponseStream *res = r->beginResponseStream("text/html");
    res->print(F("<!DOCTYPE html> <html lang=\"ja\"> <meta charset=\"utf-8\"><title>WebAPI Sample Page</title>"));
    res->print(F("<style> body {text-align: center; font-family: sans-serif;} .container {width: 480px; margin: auto; } "));
    res->print(F(".state {background: "));
    res->print(buttonOn ? "#87CEFA" : "#FF6666");
    res->print(F("; padding: 10px; margin-bottom: 10px; } "));
    res->print(F("button {font-size: 16px; margin: 4px; padding: 6px 10px; } </style>"));
    res->print(F("<body><div class=\"container\"><h2>WebAPI Sample Page</h2>"));
    res->print(F("<div class=\"state\">ボタンの状態： "));
    res->print(buttonOn ? "ON" : "OFF");
    res->print(F("</div>"));
    res->print(F("<p><button onclick=\"cmd('/toggle')\">ボタン ON / OFF</button>"));
    res->print(F("<button onclick=\"cmd('/toggle')\">"));
    res->print(buttonOn ? "ON" : "OFF");
    res->print(F("状態ボタン反転スイッチ</button></p>"));
    res->print(F("<p><button onclick=\"cmd('/on')\">ONボタン</button>"));
    res->print(F("<button onclick=\"cmd('/off')\">OFFボタン</button></p>"));
    res->print(F("<br><br></div><script>function cmd(url) { fetch(url).then(() => location.reload());} "));
    res->print(F("</script></body></html>"));
    r->send(res);
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