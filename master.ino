#include <WiFi.h>
#include <esp_now.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Ticker.h>

#define SIREN_PIN 26
#define MAX_ZONES 8
#define ENTRY_DELAY 10   // secondi
#define PIN_CODE "1234"

enum AlarmState {
  DISARMED,
  ARMED,
  ENTRY,
  ALARM
};

AlarmState alarmState = DISARMED;
bool zones[MAX_ZONES];
Ticker entryTicker;
int entryCountdown = 0;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

typedef struct {
  uint8_t node;
  uint8_t zone;
  bool state;
} ZoneMsg;

// ====== UTILS ======
const char* stateToString() {
  switch (alarmState) {
    case DISARMED: return "DISARMED";
    case ARMED: return "ARMED";
    case ENTRY: return "ENTRY DELAY";
    case ALARM: return "ALARM";
  }
  return "";
}

// ====== NOTIFY UI ======
void notifyClients() {
  StaticJsonDocument<256> doc;
  doc["state"] = stateToString();
  doc["entry"] = entryCountdown;
  JsonArray z = doc.createNestedArray("zones");
  for (int i = 0; i < MAX_ZONES; i++) z.add(zones[i]);

  String out;
  serializeJson(doc, out);
  ws.textAll(out);
}

// ====== ENTRY TIMER ======
void entryTick() {
  entryCountdown--;
  notifyClients();
  if (entryCountdown <= 0) {
    entryTicker.detach();
    alarmState = ALARM;
    digitalWrite(SIREN_PIN, HIGH);
    notifyClients();
  }
}

// ====== ESP-NOW RX ======
void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  ZoneMsg msg;
  memcpy(&msg, incomingData, sizeof(msg));
  if (msg.zone < 1 || msg.zone > MAX_ZONES) return;

  zones[msg.zone - 1] = msg.state;

  if (alarmState == ARMED && msg.state) {
    alarmState = ENTRY;
    entryCountdown = ENTRY_DELAY;
    entryTicker.attach(1, entryTick);
  }

  notifyClients();
}

// ====== WEBSOCKET ======
void onWsEvent(AsyncWebSocket *server,
               AsyncWebSocketClient *client,
               AwsEventType type,
               void *arg,
               uint8_t *data,
               size_t len) {

  if (type != WS_EVT_DATA) return;

  String msg = String((char*)data).substring(0, len);
  StaticJsonDocument<128> doc;
  deserializeJson(doc, msg);

  String cmd = doc["cmd"];
  String pin = doc["pin"];

  if (pin != PIN_CODE) return;

  if (cmd == "ARM") {
    alarmState = ARMED;
    digitalWrite(SIREN_PIN, LOW);
  }

  if (cmd == "DISARM") {
    alarmState = DISARMED;
    digitalWrite(SIREN_PIN, LOW);
    entryTicker.detach();
    entryCountdown = 0;
  }

  notifyClients();
}

void setup() {
  pinMode(SIREN_PIN, OUTPUT);
  digitalWrite(SIREN_PIN, LOW);

  WiFi.softAP("ESP32-ALARM", "12345678");

  SPIFFS.begin(true);

  esp_now_init();
  esp_now_register_recv_cb(onDataRecv);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
}

void loop() {}
