# <img src="https://upload.wikimedia.org/wikipedia/commons/8/87/Arduino_Logo.svg" alt="Arduino Logo" width="40"> Monitoramento de Pacientes e Condições de Risco 

**Componentes**: [Leonardo Castro](https://github.com/thetwelvedev), [Arthur Ramos](https://github.com/ArthurRamos26) e [Lucas Gabriel](https://github.com/lucasrocha777)

## Sumário
<!--
fazer sumário
-->

## Hands-on intermediário

- [x] Sprint 0
- [x] Sprint 1
- [ ] Sprint 2
- [ ] Pitch do projeto


## Organograma
![organograma](./imagens/organograma.png)

## Projeto Montado
![circuito](./imagens/circuito-montado.jpg)

## Documentos

#### Big Picture
![big picture](./imagens/Big%20Picture.png)

#### Plano de Teste
[Acesse aqui](./docs/Plano_de_testes_arkham.pdf)

#### Documento de Definição de Pronto(DoD)
[Acesse aqui](./docs/Definição%20de%20Pronto%20Arkham.pdf)

#### Documento de Requesitos Funcionais
[Acesse aqui](./docs/Requisitos_Funcionais_Arkham.pdf)

#### Documento de Progresso
[Acesse aqui](./docs/Documento%20de%20progresso_Arkham.pdf)

<!--
#### Slide do Pitch
[Acesse aqui](./)


#### Esqeuema de Conexões
[Acesse aqui](./imagens/)

##### Pinagem

-->

## Código do Circuito
```C
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PubSubClient.h>

// Definições de WiFi
#define WIFI_SSID "complete com seus dados"
#define WIFI_PASSWORD "complete com seus dados"

// Definições do Firebase
#define FIREBASE_HOST "complete com seus dados"
#define FIREBASE_AUTH "complete com seus dados"

// Definições do MQTT
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "monitoramento/alertas"

// Definições de sensores e atuadores
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
    
    // Conectar ao WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("WiFi conectado");

    // Configurar Firebase
    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

    // Inicializar Firebase
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);

    // Configurar MQTT
    client.setServer(MQTT_SERVER, MQTT_PORT);
    conectarMQTT();
    
    // Inicializar sensores
    dht.begin();
    Wire.begin(MPU_SDA, MPU_SCL);
    if (!mpu.begin()) {
        Serial.println("Falha ao iniciar MPU6050");
        while (1);
    }

    // Configurar pinos
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

    // Leitura dos sensores
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float angulo = a.acceleration.x;

    // Enviar dados para o Firebase
    Firebase.setFloat(firebaseData, "/dados/temperatura", temperatura);
    Firebase.setFloat(firebaseData, "/dados/umidade", umidade);
    Firebase.setFloat(firebaseData, "/dados/angulo", angulo);

    // Verificação de alertas
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

```
