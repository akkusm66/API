# ESP32 Wetterstation – API-Variante

## 📋 Projektübersicht

Diese Wetterstation basiert auf dem ESP32 und gibt Temperatur, Luftfeuchtigkeit und einen weiteren analogen Sensorwert (z. B. Joystick) über eine **API im JSON-Format** aus. Die Daten werden sinnvoll bereinigt, mit Zeitstempel versehen und über einen eingebauten Webserver bereitgestellt.

## 👥 Projektteam

- Muhammed Akkus
- Mahir Kaplan

## ✅ Features

- 📡 WiFiManager zur einfachen WLAN-Verbindung (Access Point "ESP-Wetterstation")
- 🌡 Messung von Temperatur & Luftfeuchtigkeit mit dem DHT11
- 🎮 Messung eines analogen Sensors (Joystick X-Achse)
- 🧠 Durchschnittsbildung + Ausreißerfilterung
- 🕒 Zeitstempel über NTP (configTime)
- 📊 API-Ausgabe im JSON-Format:
  - `/api/data` → alle Daten
  - `/api/data/temperature` → Temperatur
  - `/api/data/humidity` → Luftfeuchtigkeit
  - `/api/data/sensor` → Sensorwert
- 🌈 RGB-LED zeigt Status:
  - 🔴 Kein WLAN
  - 🟢 Verbunden
  - 🔵 Messung läuft
  - 🟠 Temperatur über 30 °C
- 💡 LED per Button im Webinterface ein-/ausschaltbar
- 🧪 Webinterface erreichbar unter: `http://[ESP32-IP]/`

## 🔧 Hardware-Anforderungen

- ESP32 Dev Board
- DHT11 Sensor (an GPIO2)
- Joystick (X-Achse an GPIO1)
- RGB-LED WS2812 (an GPIO8)

## 🚀 Startanleitung

1. Sketch in Arduino IDE hochladen
2. ESP32 starten → Access Point „ESP-Wetterstation“ erscheint
3. Über Webinterface mit WLAN verbinden
4. IP-Adresse im seriellen Monitor ablesen und im Browser öffnen

## 📁 API-Beispielausgabe (`/api/data`)

```json
{
  "timestamp": "2025-02-01T20:05:02",
  "temperature": 21.4,
  "humidity": 46.2,
  "sensor": 38.1
}
```

## 🧪 Tests & Ergebnisse

- LED merkt sich letzte Farbe beim Ausschalten
- Alle Sensorwerte korrekt bereinigt und synchronisiert
- API vollständig funktionsfähig
- Webinterface mit Toggle-Button für LED getestet

## 🧑‍💻 Mitwirkende

- Muhammed Akkus – Sensorik, LED-Logik, Webinterface
- Mahir Kaplan – API-Endpunkte, Tests, Formatierung