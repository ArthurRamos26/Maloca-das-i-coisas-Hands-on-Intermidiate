#include <Wire.h>
#include <MPU6050.h>
#include <DHT.h>

// Definições de sensores e atuadores
#define DHTPIN 15
#define DHTTYPE DHT11
#define LED_PIN 23
#define BUZZER_PIN 18
#define MPU_SCL 21
#define MPU_SDA 22

// Inicialização dos sensores
DHT dht(DHTPIN, DHTTYPE);
MPU6050 mpu;

// Definir o número de amostras para a média
#define NUM_SAMPLES 100

// Variáveis de média
int16_t ax_avg = 0, ay_avg = 0, az_avg = 0;

void setup() {
  // Inicia a comunicação serial
  Serial.begin(115200);
  
  // Configura pinos de LED e buzzer
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Inicia o sensor DHT11
  dht.begin();
  
  // Inicia a comunicação com o MPU6050
  Wire.begin(MPU_SDA, MPU_SCL);
  mpu.initialize();
  
  // Verifica se o MPU6050 foi inicializado corretamente
  if (!mpu.testConnection()) {
    Serial.println("Erro de conexão com o MPU6050");
    while (1);
  }
}

void loop() {
  // Lê a temperatura e umidade
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Lê os dados do acelerômetro
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  
  // Exibe as leituras no Serial Monitor
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print(" C, Umidade: ");
  Serial.print(humidity);
  Serial.print(" %, Acelerômetro: ");
  Serial.print("AX: "); Serial.print(ax);
  Serial.print(" AY: "); Serial.print(ay);
  Serial.print(" AZ: "); Serial.println(az);

  // Calcular a média das acelerações
  ax_avg = (ax_avg * (NUM_SAMPLES - 1) + ax) / NUM_SAMPLES;
  ay_avg = (ay_avg * (NUM_SAMPLES - 1) + ay) / NUM_SAMPLES;
  az_avg = (az_avg * (NUM_SAMPLES - 1) + az) / NUM_SAMPLES;

  // Verifica condições de risco
  
  // Condição 1: Temperatura de 20 à 30 graus é segura
  if (temperature < 20 || temperature > 30) {
    acionarAlerta();
  }

  // Condição 2: Umidade de 40% à 60% é segura(por conta da sala tá com mais de 60% de umidade coloquei só o abaixo de 40%)
  if (humidity < 40) {
    acionarAlerta();
  }

  // Condição 3: Movimento brusco
  // Verifica a diferença entre a aceleração atual e a média
  int threshold = 1000;  // Limite para detectar um movimento significativo
  if (abs(ax - ax_avg) > threshold || abs(ay - ay_avg) > threshold || abs(az - az_avg) > threshold) {
    acionarAlerta();
  }
  
  delay(100);  // Atraso pequeno para evitar leituras rápidas demais
}



void acionarAlerta() {
  // Aciona o buzzer
  digitalWrite(BUZZER_PIN, HIGH);
  
  // Acende o LED
  digitalWrite(LED_PIN, HIGH);
  
  // Aguarda 2 segundos para o alerta
  delay(2000);
  
  // Desliga o buzzer e o LED
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}