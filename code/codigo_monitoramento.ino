#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PubSubClient.h>

#define WIFI_SSID "Nome da rede wifi "
#define WIFI_PASSWORD "Senha da rede wifi "

#define FIREBASE_HOST "Host da rede wifi "
#define FIREBASE_AUTH "Autenticador do firebase"

#define MQTT_SERVER "Servidor MWTT "
#define MQTT_PORT 1883
#define MQTT_TOPIC "monitoramento/alertas"

#define DHTPIN 15
#define DHTTYPE DHT11
#define LED_PIN 23
#define BUZZER_PIN 18
#define MPU_SCL 21
#define MPU_SDA 22

WiFiClient espClient;
PubSubClient client(espClient);
FirebaseData firebaseData;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_MPU6050 mpu;

FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("WiFi conectado");

    // Definir a configuração do Firebase
    firebaseConfig.host = FIREBASE_HOST;
    firebaseAuth.token = FIREBASE_AUTH;
    
    Firebase.begin(firebaseConfig, firebaseAuth);

    Firebase.reconnectWiFi(true);
    client.setServer(MQTT_SERVER, MQTT_PORT);
    conectarMQTT();
    
    dht.begin();
    Wire.begin(MPU_SDA, MPU_SCL);
    if (!mpu.begin()) {
        Serial.println("Falha ao iniciar MPU6050");
        while (1);
    }
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
}

void conectarMQTT() {
    while (!client.connected()) {
        Serial.println("Conectando ao MQTT...");
        if (client.connect("ESP32Client")) {
            Serial.println("Conectado ao MQTT");
        } else {
            Serial.print("Falha, rc=");
            Serial.print(client.state());
            Serial.println(" Tentando novamente...");
            delay(5000);
        }
    }
}

void loop() {
    if (!client.connected()) {
        conectarMQTT();
    }
    client.loop();

    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float angulo = a.acceleration.x;

    Firebase.setFloat(firebaseData, "/dados/temperatura", temperatura);
    Firebase.setFloat(firebaseData, "/dados/umidade", umidade);
    Firebase.setFloat(firebaseData, "/dados/angulo", angulo);

    if (temperatura > 38 || umidade < 20 || abs(angulo) > 45) {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        client.publish(MQTT_TOPIC, "Alerta: Condição Crítica Detectada!");
    } else {
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
    }

    delay(5000);
}