
#define DEBUG

#include <PolyWiFiPortal.h>

DNSServer dnsServer;

PolyWiFiPortal::PolyWiFiPortal(AsyncWebServer& server)
  : _server(server), _apRunning(false) {}

void PolyWiFiPortal::begin(const char* staSsid,
                           const char* staPass,
                           uint8_t studentNo,
                           uint8_t staChannel) {
  _staSsid = staSsid;
  _staPass = staPass;
  _studentNo = studentNo;
  _staChannel = staChannel;

  bool conWiFi = setupWiFi();
  if (conWiFi == false) conWiFi = setupWiFi();  // もう１度だけリトライ
  if (conWiFi == false) Serial.println("Not Connect WiFi Channel No."+String(WIFI_CHANNEL_NUM));

  setupSoftAP();
  IPAddress myIP = WiFi.softAPIP();

  dnsServer.start(53, "*", myIP);

  setupRoutes();
#ifdef DEBUG
  Serial.print("  STA  IP: ");
  Serial.println(staIP());
  Serial.print("SoftAP IP: ");
  Serial.println(apIP());
  Serial.println("Start WiFi Connection.");
  Serial.println("PolyWebPortal Ver "+String(POLY_WIFI_PORTAL_VERSION));
#endif
}

bool PolyWiFiPortal::setupWiFi() {
  WiFi.setAutoReconnect(false);
  WiFi.persistent(false);
  WiFi.setSleep(false);  // あった方がタイマー関係は安定する？
  // WiFi.disconnect(true, true);

  WiFi.mode(WIFI_STA);
  // IPを指定する場合はこの段階で指定。今回は固定しない
  WiFi.begin(
    _staSsid,
    _staPass,
    WIFI_CHANNEL_NUM,       // APのチャンネル番号
    nullptr,  // bssid
    true      // true = ステルスAP
  );
#ifdef DEBUG
  Serial.println("Start WiFi Connection.");
#endif
  uint32_t start = millis();
  while (millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    wl_status_t st = WiFi.status();
    if (st == WL_CONNECTED) {
#ifdef DEBUG
      Serial.println("WiFi Connection established.");
#endif
      return true;
    }

    if (st == WL_CONNECT_FAILED || st == WL_NO_SSID_AVAIL) {
      break;  // 即再試行へ
    }
    delay(200);
  }
#ifdef DEBUG
  Serial.println("No Connection.");
#endif
  return false;
}

void PolyWiFiPortal::setupSoftAP() {
  String ssid = "ESP32-SETUP-" + String(_studentNo);
  WiFi.softAP(ssid.c_str(), SOFTAP_PASSWORD);
  _apRunning = true;
#ifdef DEBUG
  Serial.print("start ");
  Serial.println(ssid.c_str());
#endif
}

// AsyncResponseStreamで組み立てるとメモリ効率が向上する、今回は簡単のためにStringで返す
String PolyWiFiPortal::buildSoftAPPage(AsyncWebServerRequest *request) {
  String html = "<html lang=\"ja\"><head><meta charset=\"UTF-8\"><title>Poly9 WiFi Portal</title></head><body>";
    if (isFromSoftAP(request)) {
      html += "<p><strong>SoftAP 経由でアクセスしています。</strong></p>";
      html += "<p>SoftAP 経由ではインターネット接続できません。</p>";
      html += "<br>";
    }
    if (isSTAConnected()) {
      html += "<p>STA 側接続のIPアドレスは、<strong>"+WiFi.localIP().toString()+" </strong> です。</p>";
      html += "<p><a href=\"http://"+WiFi.localIP().toString()+"/index.html\" target=\"_blank\">";
      html += "STA 側ページ("+ WiFi.localIP().toString() +")にアクセス</a></p>";
      if(isSoftAPRunning()){  // SoftAPが動作して、STA側に接続できている場合は、SoftAPを停止するタイマーをセット
        esp_timer_handle_t timer;
        esp_timer_create_args_t args = {
            .callback = [](void *arg){
                PolyWiFiPortal *self = static_cast<PolyWiFiPortal*>(arg);
                self->stopSoftAP();
            },
            .arg = this,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "stop_ap_timer"
        };
        esp_timer_create(&args, &timer);
        esp_timer_start_once(timer, 2 * 1000 * 1000); // 2秒後にSoftAP停止

        html += "<p>SoftAPを停止します、WiFi接続を切り替えます。</p>";
        html += "<p>15秒後に自動的にSTA側に切り替わります...</p>";
        html += "<p>30秒以上ページが切り替わらない場合は、上のリンクをクリックしてください</p>";
        html += "<script> setTimeout(function() { window.location.href = \"http://";
        html += WiFi.localIP().toString();
        html += "/index.html\";}, 15000);</script>";
      }
    } else {
      html += "<p><strong>STA 側の接続ができていません。</strong></p>";
      html += "<p>SSIDとpasswordを確認してください。</p>";
    }
    html += "</body></html>";
    // Serial.println(html);
    return html;
}

void PolyWiFiPortal::setupRoutes() {
  // ポータル用の共通ページをラムダ式を使って定義
  auto potalhandler = [this](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", buildSoftAPPage(request));
    request->send(response);
#ifdef DEBUG
    Serial.println("called hundler "+request->url()+", sended WiFi Portal page.");
#endif
  };

  _server.on("/", HTTP_GET, potalhandler);
  _server.on("/gen_204", HTTP_GET, potalhandler);
  // ChromeのCaptive Portal Detectionは、/gen_204 を使い、ブラウザが同じなので表示できる可能性あり
  _server.on("/generate_204", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send(200,"text/plain", "OK");   // Captive Portalと判定され、自動接続しない
#ifdef DEBUG
    Serial.println("called hundler "+request->url()+", sended OK Message.");
#endif
  });

  _server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response =
      request->beginResponse(200, "image/png", poly9png, sizeof(poly9png));
    response->addHeader("Cache-Control", "max-age=31536000");
    request->send(response);
    Serial.println("called hundler "+request->url()+", sended favicon.ico.");
  });
}

bool PolyWiFiPortal::isFromSoftAP(AsyncWebServerRequest* request) const {
  return request->client()->localIP() == WiFi.softAPIP();
}

bool PolyWiFiPortal::isSTAConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

IPAddress PolyWiFiPortal::staIP() const {
  return WiFi.localIP();
}

IPAddress PolyWiFiPortal::apIP() const {
  return WiFi.softAPIP();
}
bool PolyWiFiPortal::isSoftAPRunning() {
  return _apRunning;
}

void PolyWiFiPortal::stopSoftAP(uint32_t delayMs) {
#ifdef DEBUG
  Serial.println("Stop SoftAP");
#endif
  if (WiFi.localIP() == IPAddress(0, 0, 0, 0)) return;  // STA側のIP（localIP）が無いとSoftAPを止められない

  if (!_apRunning) return;

  _apRunning = false;

  xTaskCreate(
    [](void* param) {
      uint32_t d = reinterpret_cast<uint32_t>(param);
      vTaskDelay(d / portTICK_PERIOD_MS);
      WiFi.softAPdisconnect(true);
      vTaskDelete(nullptr);
    },
    "StopSoftAP",
    2048,
    reinterpret_cast<void*>(delayMs),
    1,
    nullptr);
}