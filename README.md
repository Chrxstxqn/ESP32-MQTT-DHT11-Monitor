# ESP32 MQTT DHT11 Monitor

🌡️ **Sistema completo per il monitoraggio di temperatura e umidità tramite MQTT con ESP32 e sensore DHT11**

[![ESP32](https://img.shields.io/badge/ESP32-Compatible-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![DHT11](https://img.shields.io/badge/Sensor-DHT11-blue.svg)](https://www.adafruit.com/product/386)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange.svg)](https://mqtt.org/)
[![Arduino](https://img.shields.io/badge/Framework-Arduino-teal.svg)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)

## 📋 Descrizione

Questo progetto implementa un sistema di monitoraggio ambientale che utilizza un microcontrollore ESP32 per leggere dati di temperatura e umidità da un sensore DHT11 e pubblicarli tramite protocollo MQTT. Il sistema è progettato per essere facilmente integrato in progetti IoT e sistemi di domotica.

### 🎯 Caratteristiche Principali

- ✅ **Lettura precisa** di temperatura e umidità tramite sensore DHT11
- ✅ **Connessione WiFi** automatica con riconnessione automatica  
- ✅ **Comunicazione MQTT** con supporto per QoS e messaggi retained
- ✅ **Gestione errori** robusta con retry automatici
- ✅ **Heartbeat** e monitoraggio dello stato del dispositivo
- ✅ **Configurazione semplice** tramite file header
- ✅ **Supporto JSON** per payload strutturati
- ✅ **Debug dettagliato** con output seriale
- ✅ **Ottimizzato per basso consumo** energetico
- ✅ **Documentazione completa** in italiano

## 🛠 Hardware Richiesto

### Componenti Essenziali

| Componente | Quantità | Descrizione |
|------------|----------|-------------|
| ESP32 Development Board | 1 | Microcontrollore principale |
| Sensore DHT11 | 1 | Sensore temperatura e umidità |
| Resistenza 10kΩ | 1 | Pull-up per DHT11 (se non integrata) |
| Breadboard | 1 | Per prototipazione |
| Cavi Jumper | 3+ | Connessioni |
| Cavo USB | 1 | Programmazione e alimentazione |

### Schema di Collegamento

```
ESP32           DHT11
-----           -----
3.3V    ----    VCC (Pin 1)
GPIO4   ----    DATA (Pin 2) [con resistenza pull-up 10kΩ a 3.3V]
N/C     ----    N/C (Pin 3)  
GND     ----    GND (Pin 4)
```

## 📊 Funzionalità MQTT

### Topic Utilizzati

| Topic | Tipo | Descrizione | Payload |
|-------|------|-------------|---------|
| `esp32/dht11/temperature` | Publish | Dati temperatura | `{"value":25.4,"unit":"°C","timestamp":12345,"sensor":"DHT11"}` |
| `esp32/dht11/humidity` | Publish | Dati umidità | `{"value":65.2,"unit":"%","timestamp":12345,"sensor":"DHT11"}` |
| `esp32/status` | Publish | Stato sistema | `{"status":"online","uptime":12345,"free_heap":50000}` |
| `esp32/heartbeat` | Publish | Heartbeat | `{"timestamp":12345,"uptime_minutes":205,"status":"alive"}` |
| `esp32/command` | Subscribe | Comandi remoti | `restart`, `status` |

### Broker MQTT Supportati

- ✅ **Mosquitto** (locale o cloud)
- ✅ **HiveMQ** (pubblico: broker.hivemq.com)
- ✅ **EMQX** (pubblico: broker.emqx.io) 
- ✅ **AWS IoT Core**
- ✅ **Google Cloud IoT**
- ✅ **Azure IoT Hub**

## 🚀 Installazione e Setup

### 1. Prerequisiti

#### Opzione A: Arduino IDE
```bash
# Installa Arduino IDE da: https://www.arduino.cc/en/software

# Aggiungi supporto ESP32:
# File > Preferenze > URL Gestore Schede Aggiuntive:
# https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# Installa le librerie richieste:
# - DHT sensor library by Adafruit
# - Adafruit Unified Sensor
# - PubSubClient by Nick O'Leary
# - ArduinoJson by Benoit Blanchon
```

#### Opzione B: PlatformIO (Raccomandato)
```bash
# Installa PlatformIO CLI
pip install platformio

# Oppure usa l'estensione di VS Code:
# https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide
```

### 2. Clone del Repository

```bash
git clone https://github.com/TuoUsername/ESP32-MQTT-DHT11-Monitor.git
cd ESP32-MQTT-DHT11-Monitor
```

### 3. Configurazione

1. **Modifica il file `mqtt_config.h`** con le tue credenziali:

```cpp
// Configurazione WiFi
const char* WIFI_SSID = "TuaReteWiFi";
const char* WIFI_PASSWORD = "TuaPasswordWiFi";

// Configurazione MQTT
const char* MQTT_BROKER = "broker.hivemq.com";  // o il tuo broker
const int MQTT_PORT = 1883;
const char* MQTT_USERNAME = "";  // se richiesto
const char* MQTT_PASSWORD = "";  // se richiesto
```

### 4. Compilazione e Upload

#### Con PlatformIO:
```bash
# Compila il progetto
pio run

# Carica su ESP32
pio run --target upload

# Monitora output seriale
pio device monitor
```

#### Con Arduino IDE:
1. Apri `src/main.cpp`
2. Seleziona board: **ESP32 Dev Module**
3. Seleziona porta COM corretta
4. Click su **Upload**

## 📱 Utilizzo

### Monitoraggio Base

1. **Avvia il sistema**: L'ESP32 si connette automaticamente al WiFi e al broker MQTT
2. **Verifica connessione**: Controlla il monitor seriale per conferme di connessione
3. **Monitor dati**: I dati vengono pubblicati automaticamente ogni 10 secondi
4. **Heartbeat**: Ogni minuto viene inviato un heartbeat per monitoraggio

### Comandi Remoti

Pubblica su topic `esp32/command`:
- `restart`: Riavvia il dispositivo
- `status`: Richiede stato corrente del sistema

### Integrazione con Home Assistant

```yaml
# configuration.yaml
sensor:
  - platform: mqtt
    name: "ESP32 Temperature"
    state_topic: "esp32/dht11/temperature" 
    value_template: "{{ value_json.value }}"
    unit_of_measurement: "°C"
    
  - platform: mqtt  
    name: "ESP32 Humidity"
    state_topic: "esp32/dht11/humidity"
    value_template: "{{ value_json.value }}"
    unit_of_measurement: "%"
```

## 🔧 Personalizzazione

### Modifica Intervalli di Lettura

Nel file `mqtt_config.h`:
```cpp
#define SENSOR_PUBLISH_INTERVAL 5000    // 5 secondi invece di 10
#define HEARTBEAT_INTERVAL 30000        // 30 secondi invece di 1 minuto
```

### Aggiunta Nuovi Sensori

1. Modifica `main.cpp` per includere la libreria del nuovo sensore
2. Aggiungi funzioni di lettura nella sezione sensori
3. Crea nuovi topic MQTT in `mqtt_config.h`
4. Aggiungi pubblicazione nel loop principale

### Sicurezza TLS/SSL

Per abilitare TLS:
```cpp
// In mqtt_config.h
#define ENABLE_TLS true
const int MQTT_PORT = 8883;  // Porta TLS standard
```

## 🛡 Sicurezza

### Best Practices Implementate

- ✅ **Credenziali separate** nel file di configurazione
- ✅ **Timeout di connessione** per evitare blocchi
- ✅ **Validazione input** sui dati del sensore  
- ✅ **Gestione errori** robusta con recovery automatico
- ✅ **Last Will Testament** per notifiche di disconnessione
- ✅ **Heartbeat** per monitoraggio dello stato

### Raccomandazioni Aggiuntive

1. **Usa broker privato** invece di quelli pubblici per produzione
2. **Abilita TLS/SSL** per comunicazioni crittografate
3. **Implementa autenticazione** con username/password o certificati
4. **Filtra topic MQTT** con Access Control Lists (ACL)
5. **Aggiorna regolarmente** le librerie per patch di sicurezza

## 📈 Monitoraggio e Debug

### Log Seriale

Il sistema fornisce output dettagliato:
```
🚀 Avvio ESP32 MQTT DHT11 Monitor...
=== ESP32 MQTT DHT11 Monitor ===
Chip ID: ESP32-D0WDQ6
CPU Frequency: 240 MHz
Free Heap: 294524 bytes
WiFi MAC: AA:BB:CC:DD:EE:FF
================================

Connessione a: MiaReteWiFi
WiFi connesso!
Indirizzo IP: 192.168.1.100
Tentativo connessione MQTT... connesso!

📊 === NUOVA LETTURA ===
🌡️  Temperatura: 24.50°C
💧 Umidità: 58.20%
🔥 Indice di calore: 24.89°C
📈 Letture totali: 1 (Errori: 0)
========================
```

### Indicatori LED

- **LED fisso 2sec**: WiFi connesso
- **3 lampeggi**: MQTT connesso
- **1 lampeggio breve**: Dati pubblicati
- **2 lampeggi rapidi**: Errore sensore
- **5 lampeggi rapidi**: Errore MQTT

## 🧪 Test e Verifica

### Test Locale con Mosquitto

```bash
# Installa Mosquitto
sudo apt-get install mosquitto-clients  # Ubuntu/Debian
brew install mosquitto                   # macOS

# Ascolta tutti i topic
mosquitto_sub -h broker.hivemq.com -t "esp32/#" -v

# Invia comando di test
mosquitto_pub -h broker.hivemq.com -t "esp32/command" -m "status"
```

### Verifica Funzionamento

- [ ] ESP32 si connette al WiFi
- [ ] Connessione MQTT stabilita  
- [ ] Dati pubblicati regolarmente
- [ ] Heartbeat inviato ogni minuto
- [ ] Comandi remoti funzionanti
- [ ] Riconnessione automatica funzionante

## 📚 Esempi Avanzati

### Integrazione con Node-RED

```json
[
    {
        "id": "mqtt-in",
        "type": "mqtt in", 
        "topic": "esp32/dht11/+",
        "qos": "1",
        "broker": "your-broker-config"
    },
    {
        "id": "json-parse",
        "type": "json"
    },
    {
        "id": "gauge",
        "type": "ui_gauge",
        "min": 0,
        "max": 100
    }
]
```

### Dashboard con Grafana

```sql
-- Query per InfluxDB
SELECT mean("value") 
FROM "temperature" 
WHERE time >= now() - 1h 
GROUP BY time(5m)
```

## 🤝 Contribuire

1. Fork del repository
2. Crea branch per la tua feature (`git checkout -b feature/amazing-feature`)
3. Commit delle modifiche (`git commit -m 'Add amazing feature'`)  
4. Push del branch (`git push origin feature/amazing-feature`)
5. Apri una Pull Request

## 📝 Changelog

### v1.0.0 (2024-11-05)
- ✨ Rilascio iniziale
- ✅ Supporto DHT11 con ESP32
- ✅ Comunicazione MQTT completa
- ✅ Documentazione italiana
- ✅ Esempi di integrazione

## 📄 Licenza

Questo progetto è rilasciato sotto licenza MIT. Vedi il file [LICENSE](LICENSE) per i dettagli.

## 🔗 Link Utili

- [Documentazione ESP32](https://docs.espressif.com/projects/esp32/en/latest/)
- [MQTT.org](https://mqtt.org/)
- [PlatformIO](https://platformio.org/)
- [Arduino IDE](https://www.arduino.cc/en/software)
- [DHT11 Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)

---

**⭐ Se questo progetto ti è stato utile, lascia una stella su GitHub!**

Made with ❤️ by Chrxstxqn
