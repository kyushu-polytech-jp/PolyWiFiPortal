#pragma once

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#include "PolyWiFiPortal_config.h"

#define POLY_WIFI_PORTAL_VERSION "0.3"

class PolyWiFiPortal {
public:
  PolyWiFiPortal(AsyncWebServer& server);

  // 出席番号 n を渡す（SoftAP: 192.168.n.1）
  void begin(const char* staSsid,
             const char* staPass,
             uint8_t studentNo,
             uint8_t staChannel = WIFI_CHANNEL_NUM);

  bool isSTAConnected() const;
  IPAddress staIP() const;
  IPAddress apIP() const;

  void stopSoftAP(uint32_t delayMs = 1000);
  bool setupWiFi();

  bool isSoftAPRunning();

private:
  AsyncWebServer& _server;

  const char* _staSsid;
  const char* _staPass;
  uint8_t _studentNo;
  uint8_t _staChannel;

  bool _apRunning;

  //bool setupWiFi();
  //void connectWiFi();
  void setupSoftAP();
  void setupRoutes();

  bool isFromSoftAP(AsyncWebServerRequest* request) const;
  String buildSoftAPPage(AsyncWebServerRequest *request);
};
