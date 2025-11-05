/*
 * ESP32 MQTT DHT11 Temperature & Humidity Monitor
 * 
 * Questo progetto implementa un sistema di monitoraggio della temperatura
 * e umidità utilizzando un sensore DHT11 collegato a un ESP32, con
 * pubblicazione dei dati tramite protocollo MQTT.
 * 
 * Autore: ESP32-MQTT Guide
 * Versione: 1.0.0
 * Data: November 2024
 * 
 * Hardware richiesto:
 * - ESP32 Development Board
 * - Sensore DHT11
 * - Resistenza pull-up 10kΩ (se non inclusa nel modulo DHT11)
 * - Breadboard e cavi jumper
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "mqtt_config.h"

// ==================== CONFIGURAZIONE HARDWARE ====================

// Pin di connessione del DHT11
#define DHTPIN 4          // GPIO4 per il pin dati del DHT11
#define DHTTYPE DHT11     // Tipo di sensore DHT utilizzato

// LED integrato per indicazioni di stato
#define LED_BUILTIN 2

// ==================== CONFIGURAZIONE MQTT ====================

// Topics MQTT per la pubblicazione dei dati
const char* MQTT_TOPIC_TEMPERATURE = "esp32/dht11/temperature";
const char* MQTT_TOPIC_HUMIDITY = "esp32/dht11/humidity";
const char* MQTT_TOPIC_STATUS = "esp32/status";
const char* MQTT_TOPIC_HEARTBEAT = "esp32/heartbeat";

// QoS level per i messaggi MQTT
const int MQTT_QOS = 1;

// ==================== INIZIALIZZAZIONE OGGETTI ====================

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ==================== VARIABILI GLOBALI ====================

// Timing per le letture del sensore
unsigned long lastSensorReading = 0;
const unsigned long SENSOR_INTERVAL = 10000; // 10 secondi

// Timing per heartbeat
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 60000; // 1 minuto

// Variabili per memorizzare le ultime letture
float lastTemperature = 0.0;
float lastHumidity = 0.0;

// Contatori per statistiche
unsigned long totalReadings = 0;
unsigned long failedReadings = 0;

// ==================== FUNZIONI DI UTILITÀ ====================

/**
 * Lampeggia il LED integrato per indicare lo stato
 * @param times Numero di lampeggi
 * @param delayMs Durata di ogni lampeggio in millisecondi
 */
void blinkLED(int times, int delayMs = 200) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_BUILTIN, LOW);
        delay(delayMs);
    }
}

/**
 * Stampa informazioni di sistema sulla seriale
 */
void printSystemInfo() {
    Serial.println("\n=== ESP32 MQTT DHT11 Monitor ===");
    Serial.print("Chip ID: ");
    Serial.println(ESP.getChipModel());
    Serial.print("CPU Frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");
    Serial.print("Free Heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
    Serial.print("WiFi MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("================================\n");
}

// ==================== CONNESSIONE WIFI ====================

/**
 * Connette l'ESP32 alla rete WiFi configurata
 */
void setupWiFi() {
    delay(10);
    Serial.println();
    Serial.print("Connessione a: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    const int maxAttempts = 20;

    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        Serial.print(".");
        blinkLED(1, 100);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi connesso!");
        Serial.print("Indirizzo IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal Strength (RSSI): ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
        
        // LED fisso per 2 secondi per indicare connessione riuscita
        digitalWrite(LED_BUILTIN, HIGH);
        delay(2000);
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        Serial.println();
        Serial.println("❌ Impossibile connettersi al WiFi!");
        Serial.println("Verificare le credenziali in mqtt_config.h");
        // Lampeggio rapido per indicare errore
        blinkLED(10, 100);
    }
}

// ==================== CONNESSIONE MQTT ====================

/**
 * Callback chiamata quando arriva un messaggio MQTT
 * @param topic Topic del messaggio ricevuto
 * @param payload Contenuto del messaggio
 * @param length Lunghezza del messaggio
 */
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Messaggio ricevuto [");
    Serial.print(topic);
    Serial.print("]: ");
    
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    
    // Gestisci comandi remoti se necessario
    if (String(topic) == "esp32/command") {
        if (message == "restart") {
            Serial.println("Comando restart ricevuto. Riavvio in 3 secondi...");
            delay(3000);
            ESP.restart();
        } else if (message == "status") {
            publishStatus();
        }
    }
}

/**
 * Connette l'ESP32 al broker MQTT
 */
void connectToMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Tentativo connessione MQTT...");
        
        // Crea un client ID unico basato sul MAC address
        String clientId = "ESP32-DHT11-";
        clientId += String(random(0xffff), HEX);
        
        // Messaggio Last Will and Testament
        const char* willTopic = MQTT_TOPIC_STATUS;
        const char* willMessage = "{\"status\":\"offline\",\"timestamp\":" + String(millis()) + "}";
        
        if (mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD, 
                              willTopic, MQTT_QOS, true, willMessage)) {
            Serial.println(" connesso!");
            
            // Pubblica messaggio di connessione
            publishStatus();
            
            // Iscriviti ai topic di comando se necessario
            mqttClient.subscribe("esp32/command");
            
            blinkLED(3, 200); // 3 lampeggi per indicare connessione MQTT
            
        } else {
            Serial.print(" fallito, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" riprovo tra 5 secondi");
            
            blinkLED(5, 100); // Lampeggio rapido per errore MQTT
            delay(5000);
        }
    }
}

// ==================== PUBBLICAZIONE DATI ====================

/**
 * Pubblica lo stato del sistema
 */
void publishStatus() {
    DynamicJsonDocument doc(256);
    doc["status"] = "online";
    doc["uptime"] = millis();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["total_readings"] = totalReadings;
    doc["failed_readings"] = failedReadings;
    
    String payload;
    serializeJson(doc, payload);
    
    mqttClient.publish(MQTT_TOPIC_STATUS, payload.c_str(), true);
    Serial.println("✅ Status pubblicato");
}

/**
 * Pubblica i dati di temperatura e umidità
 * @param temperature Temperatura in gradi Celsius
 * @param humidity Umidità relativa in percentuale
 */
void publishSensorData(float temperature, float humidity) {
    // Crea timestamp
    unsigned long timestamp = millis();
    
    // Pubblica temperatura
    DynamicJsonDocument tempDoc(128);
    tempDoc["value"] = temperature;
    tempDoc["unit"] = "°C";
    tempDoc["timestamp"] = timestamp;
    tempDoc["sensor"] = "DHT11";
    
    String tempPayload;
    serializeJson(tempDoc, tempPayload);
    
    if (mqttClient.publish(MQTT_TOPIC_TEMPERATURE, tempPayload.c_str(), false)) {
        Serial.print("🌡️  Temperatura pubblicata: ");
        Serial.print(temperature);
        Serial.println("°C");
    }
    
    // Pubblica umidità
    DynamicJsonDocument humDoc(128);
    humDoc["value"] = humidity;
    humDoc["unit"] = "%";
    humDoc["timestamp"] = timestamp;
    humDoc["sensor"] = "DHT11";
    
    String humPayload;
    serializeJson(humDoc, humPayload);
    
    if (mqttClient.publish(MQTT_TOPIC_HUMIDITY, humPayload.c_str(), false)) {
        Serial.print("💧 Umidità pubblicata: ");
        Serial.print(humidity);
        Serial.println("%");
    }
}

/**
 * Pubblica heartbeat per monitoraggio
 */
void publishHeartbeat() {
    DynamicJsonDocument doc(128);
    doc["timestamp"] = millis();
    doc["uptime_minutes"] = millis() / 60000;
    doc["status"] = "alive";
    
    String payload;
    serializeJson(doc, payload);
    
    mqttClient.publish(MQTT_TOPIC_HEARTBEAT, payload.c_str(), false);
    Serial.println("💓 Heartbeat inviato");
}

// ==================== LETTURA SENSORI ====================

/**
 * Legge i dati dal sensore DHT11 e li pubblica via MQTT
 */
void readAndPublishSensorData() {
    // Leggi umidità e temperatura dal DHT11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    totalReadings++;
    
    // Controlla se le letture sono valide
    if (isnan(humidity) || isnan(temperature)) {
        failedReadings++;
        Serial.println("❌ Errore lettura sensore DHT11!");
        
        // Lampeggio di errore
        blinkLED(2, 50);
        return;
    }
    
    // Calcola indice di calore (Heat Index)
    float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
    
    // Stampa letture su seriale
    Serial.println("\n📊 === NUOVA LETTURA ===");
    Serial.print("🌡️  Temperatura: ");
    Serial.print(temperature);
    Serial.println("°C");
    Serial.print("💧 Umidità: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("🔥 Indice di calore: ");
    Serial.print(heatIndex);
    Serial.println("°C");
    Serial.print("📈 Letture totali: ");
    Serial.print(totalReadings);
    Serial.print(" (Errori: ");
    Serial.print(failedReadings);
    Serial.println(")");
    Serial.println("========================\n");
    
    // Pubblica i dati solo se sono cambiati significativamente
    // (riduce il traffico MQTT per valori stabili)
    if (abs(temperature - lastTemperature) > 0.5 || 
        abs(humidity - lastHumidity) > 2.0 ||
        totalReadings == 1) { // Prima lettura
        
        publishSensorData(temperature, humidity);
        
        // Salva le ultime letture
        lastTemperature = temperature;
        lastHumidity = humidity;
        
        // Breve lampeggio per indicare pubblicazione
        blinkLED(1, 50);
    } else {
        Serial.println("📊 Valori stabili - pubblicazione saltata");
    }
}

// ==================== SETUP E LOOP PRINCIPALE ====================

/**
 * Funzione di setup - eseguita una volta all'avvio
 */
void setup() {
    // Inizializza comunicazione seriale
    Serial.begin(115200);
    Serial.println("\n🚀 Avvio ESP32 MQTT DHT11 Monitor...");
    
    // Inizializza LED integrato
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    // Stampa informazioni di sistema
    printSystemInfo();
    
    // Inizializza sensore DHT11
    dht.begin();
    Serial.println("✅ Sensore DHT11 inizializzato");
    
    // Connetti al WiFi
    setupWiFi();
    
    // Configura client MQTT
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setKeepAlive(60);
    mqttClient.setSocketTimeout(30);
    
    // Genera seed per numeri casuali
    randomSeed(micros());
    
    Serial.println("🎯 Setup completato! Inizio monitoraggio...");
    Serial.println("📊 Intervallo letture sensore: " + String(SENSOR_INTERVAL/1000) + " secondi");
    Serial.println("💓 Intervallo heartbeat: " + String(HEARTBEAT_INTERVAL/1000) + " secondi\n");
}

/**
 * Loop principale - eseguito continuamente
 */
void loop() {
    // Mantieni connessione MQTT attiva
    if (!mqttClient.connected()) {
        connectToMQTT();
    }
    mqttClient.loop();
    
    // Controlla connessione WiFi
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi disconnesso! Tentativo riconnessione...");
        setupWiFi();
    }
    
    unsigned long currentMillis = millis();
    
    // Leggi sensore e pubblica dati
    if (currentMillis - lastSensorReading >= SENSOR_INTERVAL) {
        lastSensorReading = currentMillis;
        readAndPublishSensorData();
    }
    
    // Pubblica heartbeat
    if (currentMillis - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        lastHeartbeat = currentMillis;
        publishHeartbeat();
    }
    
    // Breve pausa per evitare sovraccarico del processore
    delay(100);
}