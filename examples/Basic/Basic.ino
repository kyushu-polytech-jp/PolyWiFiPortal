#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PolyWiFiPortal.h>

#define WiFi_SSID "YOUR_SSID"
#define WiFi_PASS "YOUR_PASSWORD"

AsyncWebServer server(80);
PolyWiFiPortal portal(server);

// webAPIで引き渡すメッセージの定義  get?temp=1234
const char *PARAM_MESSAGE = "temp";

void setup() {
  Serial.begin(115200);

  portal.begin(WiFi_SSID, WiFi_PASS, 99);  // ← 最後は識別番号(2桁)
  // WiFi接続：IPアドレスはシリアル通信で表示  or  http://192.168.4.1/からページ切り替え
  // IP不明ならWeb(SSID：ESP32-SETUP-番号/ パスワードは"12345678"）に接続し、http://192.168.4.1へ
  // url: "/" は、ライブラリシステム側でIPアドレス表示に使用している
  // url: "/" にアクセスすると、SoftAPは自動停止する（直接index.html等にアクセスするとSoftAPは継続して利用できる）
  // url: "/" は、/index.html へ切り替える

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html;  // htmlにindex.htmlを定義する
    html += "<html><title>Hello Web API</title><body>";
    html += "<h2>Hello Web API page by ESP32</h2>";
    html += "<p>Sample index page http://" + WiFi.localIP().toString() + "/index.html(this page)</p>";
    html += "<p><a href=\"http://" + WiFi.localIP().toString() + "/index.html\" target=\"_blank\">this page</a></p><br>";
    html += "<h2>Hello Web API World by ESP32</h2>";
    html += "<p>Sample Web API : http://" + WiFi.localIP().toString() + "/get?temp=123.456</p>";
    html += "<p><a href=\"http://" + WiFi.localIP().toString() + "/set?temp=123.456\" target=\"_blank\">STA IP addr(" + WiFi.localIP().toString() + ")/set?temp=123.456</a></p>";
    html += "</body></html>";

    AsyncWebServerResponse *res = request->beginResponse(200, "text/html", html);  // html をレスポンスデータ作成用の領域にコピーする
    request->send(res);
    Serial.println("called URL:"+request->url()+" , sended index.html.");
  });

  // Send a GET request to <IP>/set?message=<message>
  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
    AsyncWebServerResponse *res = request->beginResponse(200, "text/plain", "GET method called path:/set temp=" + message);
    request->send(res);

    Serial.println("GET method called path:/set temp=" + message);
  });

  // 宛先不明のパス要求があったらここで返事する
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
    Serial.println("called URL:" + request->url() + " , sended 404 Not found.");
  });

  server.begin();
}

void loop() {
  if (portal.isSoftAPRunning()) Serial.println("SoftAPRunning.");
  delay(5000);
  // Asyncなので空でOK
}