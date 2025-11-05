/*
 * Configurazione MQTT e WiFi per ESP32 DHT11 Monitor
 * 
 * IMPORTANTE: Modifica questo file con le tue credenziali prima di compilare!
 * 
 * Autore: Christian Schito
 * Versione: 1.0.0
 */

#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// ==================== CONFIGURAZIONE WIFI ====================

// ⚠️  MODIFICA QUESTI VALORI CON LE TUE CREDENZIALI WiFi
const char* WIFI_SSID = "TuoNomeWiFi";           // Nome della rete WiFi
const char* WIFI_PASSWORD = "TuaPasswordWiFi";   // Password WiFi

// ==================== CONFIGURAZIONE MQTT BROKER ====================

// Configurazione Broker MQTT
// Esempi di broker pubblici per test:
// - broker.hivemq.com (porta 1883)
// - test.mosquitto.org (porta 1883)
// - broker.emqx.io (porta 1883)

const char* MQTT_BROKER = "broker.hivemq.com";   // Indirizzo broker MQTT
const int MQTT_PORT = 1883;                      // Porta broker MQTT (1883 standard, 8883 per TLS)

// Credenziali MQTT (lascia vuoto se il broker non richiede autenticazione)
const char* MQTT_USERNAME = "";                 // Username MQTT (opzionale)
const char* MQTT_PASSWORD = "";                 // Password MQTT (opzionale)

// ==================== CONFIGURAZIONE AVANZATA ====================

// Timeout di connessione (millisecondi)
#define WIFI_CONNECTION_TIMEOUT 20000    // 20 secondi
#define MQTT_CONNECTION_TIMEOUT 10000    // 10 secondi

// Intervalli di pubblicazione (millisecondi)
#define SENSOR_PUBLISH_INTERVAL 10000    // 10 secondi tra le letture
#define HEARTBEAT_INTERVAL 60000         // 1 minuto per heartbeat
#define STATUS_UPDATE_INTERVAL 300000    // 5 minuti per aggiornamenti di stato

// Configurazione sensore DHT11
#define DHT_READ_TIMEOUT 2000           // Timeout lettura sensore (ms)
#define MIN_TEMP_CHANGE 0.5             // Variazione minima temperatura per pubblicazione
#define MIN_HUM_CHANGE 2.0              // Variazione minima umidità per pubblicazione

// ==================== CONFIGURAZIONE SICUREZZA ====================

// Abilita TLS/SSL (richiede broker con supporto TLS sulla porta 8883)
#define ENABLE_TLS false

// Certificate Authority per TLS (se abilitato)
// Sostituisci con il certificato del tuo broker MQTT
#ifdef ENABLE_TLS
const char* CA_CERT = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----\n";
#endif

// ==================== CONFIGURAZIONE DEBUG ====================

// Abilita output debug dettagliato
#define DEBUG_MODE true

// Abilita statistiche dettagliate
#define ENABLE_STATISTICS true

// Abilita log su seriale
#define SERIAL_DEBUG true
#define SERIAL_BAUD_RATE 115200

// ==================== TOPIC MQTT PERSONALIZZABILI ====================

// Prefisso per tutti i topic MQTT
#define MQTT_TOPIC_PREFIX "esp32/dht11"

// Topic specifici (verranno combinati con il prefisso)
#define TOPIC_TEMPERATURE "/temperature"
#define TOPIC_HUMIDITY "/humidity" 
#define TOPIC_STATUS "/status"
#define TOPIC_HEARTBEAT "/heartbeat"
#define TOPIC_COMMAND "/command"

// Quality of Service MQTT (0, 1, o 2)
#define MQTT_QOS_LEVEL 1

// Retain messages (true/false)
#define MQTT_RETAIN_MESSAGES false

// ==================== CONFIGURAZIONE DISPOSITIVO ====================

// Identificativo unico del dispositivo
// Può essere personalizzato o generato automaticamente
#define DEVICE_ID_AUTO true              // Se true, usa MAC address come ID
#define DEVICE_ID_CUSTOM "ESP32-DHT11-001"  // ID personalizzato se AUTO è false

// Informazioni dispositivo per identificazione
#define DEVICE_NAME "ESP32 DHT11 Monitor"
#define DEVICE_VERSION "1.0.0"
#define DEVICE_MANUFACTURER "DIY Project"

#endif // MQTT_CONFIG_H
