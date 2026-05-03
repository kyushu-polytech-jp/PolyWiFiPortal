#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFi.scanNetworks(true);  // 'true' turns Async Mode ON

  delay(5000);

  int16_t networksFound = WiFi.scanComplete();
      Serial.println("No | Ch | RSSI | SSID    \t\t\t\t| encryption"); 
  for (int i = 0; i < networksFound; ++i) {
    Serial.printf("%2d", i + 1);
    Serial.print(" | ");
    Serial.printf("%2ld", WiFi.channel(i));
    Serial.print(" | ");
    Serial.printf("%4ld", WiFi.RSSI(i));
    Serial.print(" | ");
    Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
    Serial.print(" \t| ");
    switch (WiFi.encryptionType(i)) {
      case WIFI_AUTH_OPEN: Serial.print("open"); break;
      case WIFI_AUTH_WEP: Serial.print("WEP"); break;
      case WIFI_AUTH_WPA_PSK: Serial.print("WPA"); break;
      case WIFI_AUTH_WPA2_PSK: Serial.print("WPA2"); break;
      case WIFI_AUTH_WPA_WPA2_PSK: Serial.print("WPA+WPA2"); break;
      case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
      case WIFI_AUTH_WPA3_PSK: Serial.print("WPA3"); break;
      case WIFI_AUTH_WPA2_WPA3_PSK: Serial.print("WPA2+WPA3"); break;
      case WIFI_AUTH_WAPI_PSK: Serial.print("WAPI"); break;
      default: Serial.print("unknown");
    }
    Serial.println();
    delay(10);
  }
  Serial.println("");
  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();
}

void loop() {
}
