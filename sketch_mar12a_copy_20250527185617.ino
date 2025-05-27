#include <WiFiManager.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define JOY_X_PIN 1
#define LED_PIN 8
#define LED_COUNT 1

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);
Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

bool ledEnabled = true;
uint8_t lastR = 0, lastG = 0, lastB = 0;

void setLED(uint8_t r, uint8_t g, uint8_t b) {
  lastR = r;
  lastG = g;
  lastB = b;
  if (ledEnabled) {
    led.setPixelColor(0, r, g, b);
  } else {
    led.setPixelColor(0, 0, 0, 0);
  }
  led.show();
}

void handleToggleLED() {
  ledEnabled = !ledEnabled;
  setLED(lastR, lastG, lastB);
  server.send(200, "text/plain", ledEnabled ? "LED an" : "LED aus");
}

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <title>ESP32 Wetterstation</title>
      <style>
        body { font-family: sans-serif; text-align: center; margin-top: 30px; }
        button { font-size: 18px; padding: 10px 20px; margin: 20px; }
      </style>
    </head>
    <body>
      <h1>🌤 ESP32 Wetterstation</h1>
      <button onclick="toggleLED()">RGB-LED An/Aus</button>
      <p id="status">Status wird geladen...</p>

      <script>
        async function toggleLED() {
          const res = await fetch('/toggleLED');
          const text = await res.text();
          document.getElementById('status').innerText = 'LED-Status: ' + text;
        }

        window.onload = toggleLED;
      </script>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  dht.begin();
  pinMode(JOY_X_PIN, INPUT);

  led.begin();
  led.setBrightness(100);
  setLED(255, 0, 0); // 🔴 kein WLAN

  WiFiManager wm;
  wm.setConfigPortalBlocking(true);
  wm.setConfigPortalTimeout(0);
  bool res = wm.startConfigPortal("ESP-Wetterstation");

  if (!res) {
    Serial.println("❌ WLAN nicht verbunden. Neustart...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("✅ WLAN verbunden!");
  Serial.print("🌐 IP-Adresse: ");
  Serial.println(WiFi.localIP());
  setLED(0, 255, 0); // 🟢 verbunden

  configTime(0, 0, "pool.ntp.org");

  server.on("/", handleRoot);
  server.on("/toggleLED", handleToggleLED);
  server.on("/api/data", handleAllData);
  server.on("/api/data/temperature", handleTemperature);
  server.on("/api/data/humidity", handleHumidity);
  server.on("/api/data/sensor", handleSensor);
  server.begin();
}

void loop() {
  server.handleClient();
}

String getTimestamp() {
  time_t now = time(nullptr);
  struct tm* tm_info = localtime(&now);
  char buffer[30];
  strftime(buffer, 30, "%Y-%m-%dT%H:%M:%S", tm_info);
  return String(buffer);
}

void handleAllData() {
  setLED(0, 0, 255); // 🔵 Messung läuft

  float t = 0, h = 0;
  int gueltig = 0;
  for (int i = 0; i < 3; i++) {
    float tt = dht.readTemperature();
    float hh = dht.readHumidity();
    if (!isnan(tt) && tt > -20 && tt < 60 && !isnan(hh) && hh >= 0 && hh <= 100) {
      t += tt;
      h += hh;
      gueltig++;
    }
    delay(200);
  }

  float temperatur = gueltig > 0 ? t / gueltig : NAN;
  float hum = gueltig > 0 ? h / gueltig : NAN;
  float s = analogRead(JOY_X_PIN) * (100.0 / 4095.0);
  String ts = getTimestamp();

  if (!isnan(temperatur) && temperatur > 30.0)
    setLED(255, 128, 0); // 🟠 zu heiß
  else
    setLED(0, 255, 0);   // 🟢 normal

  String json = "{";
  json += "\"timestamp\":\"" + ts + "\",";
  json += "\"temperature\":" + String(temperatur, 1) + ",";
  json += "\"humidity\":" + String(hum, 1) + ",";
  json += "\"sensor\":" + String(s, 1);
  json += "}";

  server.send(200, "application/json", json);
}

void handleTemperature() {
  float t = 0;
  int gueltig = 0;
  for (int i = 0; i < 3; i++) {
    float tt = dht.readTemperature();
    if (!isnan(tt) && tt > -20 && tt < 60) {
      t += tt;
      gueltig++;
    }
    delay(200);
  }
  float temperatur = gueltig > 0 ? t / gueltig : NAN;
  String ts = getTimestamp();

  if (!isnan(temperatur) && temperatur > 30.0)
    setLED(255, 128, 0);
  else
    setLED(0, 0, 255);

  String json = "{";
  json += "\"timestamp\":\"" + ts + "\",";
  json += "\"temperature\":" + String(temperatur, 1);
  json += "}";
  server.send(200, "application/json", json);
}

void handleHumidity() {
  float h = 0;
  int gueltig = 0;
  for (int i = 0; i < 3; i++) {
    float hh = dht.readHumidity();
    if (!isnan(hh) && hh >= 0 && hh <= 100) {
      h += hh;
      gueltig++;
    }
    delay(200);
  }
  float hum = gueltig > 0 ? h / gueltig : NAN;
  String ts = getTimestamp();

  String json = "{";
  json += "\"timestamp\":\"" + ts + "\",";
  json += "\"humidity\":" + String(hum, 1);
  json += "}";
  server.send(200, "application/json", json);
}

void handleSensor() {
  float s = analogRead(JOY_X_PIN) * (100.0 / 4095.0);
  String ts = getTimestamp();

  String json = "{";
  json += "\"timestamp\":\"" + ts + "\",";
  json += "\"sensor\":" + String(s, 1);
  json += "}";
  server.send(200, "application/json", json);
}
