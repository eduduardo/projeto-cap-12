#include <DHT.h>

#define DHTPIN 23     // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22 // Tipo do sensor DHT
#define RELAY_PIN 5   // Pino do relé

#define TRIG_PIN 2 // Pino Trigger do HC-SR04
#define ECHO_PIN 4 // Pino Echo do HC-SR04

#define PIR_PIN 14    // Pino do PIR
#define BUZZER_PIN 16 // Pino do buzzer

#define LDR_PIN 34 // Pin do LDR

DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;
unsigned long relayOnTime = 0;
unsigned long relayOffTime = 0;
unsigned long dhtCheckInterval = 2000; // Intervalo para verificar o DHT

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Começa com o relé desligado

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Começa com o buzzer desligado
}

void loop() {
  unsigned long currentMillis = millis();
  delay(2000);

  // Verifica a presença usando o sensor PIR
  detectPresence();

  // Leitura de temperatura e umidade
  if (currentMillis - previousMillis >= dhtCheckInterval) {
    previousMillis = currentMillis;
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Celsius

    // Verifica se a leitura falhou
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Falha ao ler do DHT sensor!");
      return;
    }

    // Exibe os dados no Serial Monitor
    Serial.print("Umidade: ");
    Serial.print(humidity);
    Serial.print("%  Temperatura: ");
    Serial.print(temperature);
    Serial.println("°C");

    // Lê o nível de água
    float waterLevel = getWaterLevel();
    Serial.print("Nível de água: ");
    Serial.print(waterLevel);
    Serial.println(" cm");

    // Lê a luminosidade do LDR
    int ldrValue = analogRead(LDR_PIN);
    Serial.print("Luminosidade (LDR): ");
    Serial.println(ldrValue);

    // Controle de irrigação baseado na temperatura, umidade e nível de água
    if (waterLevel > 200) { // Nível de água baixo (quando a distancia entre o sensor e a água é maior que 200cm)
      Serial.println("Nível de água baixo! Irrigação não ativada.");
      digitalWrite(RELAY_PIN, LOW); // Garante que o relé esteja desligado
    } else {
      // Ajuste do tempo de ativação do relé com base na luminosidade
      if (ldrValue > 700) { // Luz alta (ajuste conforme necessário)
        Serial.println("Luz alta! Aumentando tempo de irrigação.");
        relayOnTime += 5 * 60000; // Aumenta o tempo de ativação do relé (5 minutos)
      } else if (ldrValue < 300) { // Luz baixa (ajuste conforme necessário)
        Serial.println("Luz baixa! Diminuindo tempo de irrigação.");
        relayOffTime += 5 * 60000; // Aumenta o tempo de desativação do relé (5 minutos)
      }

      // Ajuste do tempo de ativação do relé com base na temperatura
      if (temperature > 30) { // Temperatura alta
        Serial.println("Temperatura alta! Aumentando tempo de irrigação.");
        relayOnTime += 5 * 60000; // Aumenta o tempo de ativação do relé (5 minutos)
      } else if (temperature < 20) { // Temperatura baixa
        Serial.println("Temperatura baixa! Diminuindo tempo de irrigação.");
        relayOffTime += 5 * 60000; // Aumenta o tempo de desativação do relé (5 minutos)
      }

      // Controle baseado na umidade
      if (humidity < 40) { // Umidade baixa
        Serial.println("Umidade baixa! Ativando irrigação.");
        digitalWrite(RELAY_PIN, HIGH); // Liga o relé
        relayOnTime = currentMillis; // Marca o tempo de ativação do relé
      } else if (humidity > 60) { // Umidade alta
        Serial.println("Umidade alta! Desativando irrigação.");
        digitalWrite(RELAY_PIN, LOW); // Desliga o relé
      } else {
        // Se a umidade estiver dentro do intervalo desejado, usa os tempos calculados
        if (currentMillis - relayOnTime >= relayOnTime) {
          digitalWrite(RELAY_PIN, LOW); // Desliga o relé
          relayOffTime = currentMillis; // Marca o tempo de desativação do relé
        }
        if (currentMillis - relayOffTime >= relayOffTime) {
          digitalWrite(RELAY_PIN, HIGH); // Liga o relé
          relayOnTime = currentMillis; // Marca o tempo de ativação do relé
        }
      }
    }
  }
}

float getWaterLevel() {
  // Envia um pulso de 10µs para o Trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Lê o tempo que o pulso leva para voltar
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calcula a distância (em cm)
  float distance = duration * 0.034 / 2; // Divide por 2 porque a medição vai e volta
  return distance;
}

void detectPresence() {
  int pirState = digitalRead(PIR_PIN);

  if (pirState == HIGH) { // Movimento detectado
    Serial.println("Movimento detectado! Ativando sistema de segurança.");
    digitalWrite(BUZZER_PIN, HIGH); // Ativa o buzzer
    delay(1000); // Atraso para o buzzer tocar
    digitalWrite(BUZZER_PIN, LOW); // Desativa o buzzer
  } else {
    Serial.println("Nenhum movimento.");
  }
}