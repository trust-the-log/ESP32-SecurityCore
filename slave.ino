#include <WiFi.h>
#include <esp_now.h>

#define NODE_ID 1
#define ZONE_ID 1
#define ZONE_PIN 14

uint8_t MASTER_MAC[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC};

typedef struct {
  uint8_t node;
  uint8_t zone;
  bool state;
} ZoneMsg;

void sendState(bool state) {
  ZoneMsg msg = {NODE_ID, ZONE_ID, state};
  esp_now_send(MASTER_MAC, (uint8_t*)&msg, sizeof(msg));
}

void setup() {
  pinMode(ZONE_PIN, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_peer_info_t peer{};
  memcpy(peer.peer_addr, MASTER_MAC, 6);
  esp_now_add_peer(&peer);
}

void loop() {
  static bool last = HIGH;
  bool now = digitalRead(ZONE_PIN);
  if (now != last) {
    sendState(now == LOW);
    last = now;
  }
  delay(50);
}
