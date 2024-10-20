#include <DHT.h>

#define DHTPIN 23        // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22   // Tipo do sensor DHT
#define RELAY_PIN 5     // Pino do relé


#define TRIG_PIN 2      // Pino Trigger do HC-SR04
#define ECHO_PIN 4      // Pino Echo do HC-SR04

#define PIR_PIN 17       // Pino do sensor PIR
#define BUZZER_PIN 16 // pino do buzzer



DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Começa com o relé desligado

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(PIR_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Começa com o buzzer desligado
}

void loop() {
  delay(2000); // Atraso para evitar leituras muito frequentes

  // Verifica a presença usando o sensor PIR
  detectPresence();

  // Leitura de temperatura e umidade
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

  // Controle de irrigação baseado na temperatura, umidade e nível de água
  if (waterLevel < 10) { // Nível de água baixo
    Serial.println("Nível de água baixo! Irrigação não ativada.");
    digitalWrite(RELAY_PIN, LOW); // Garante que o relé esteja desligado
    return;
  } 

  // Controle de irrigação baseado na temperatura e umidade
  if (humidity < 40) { // Umidade baixa
    Serial.println("Umidade baixa! Ativando irrigação.");
    digitalWrite(RELAY_PIN, HIGH); // Liga o relé
  } else if (humidity > 60) { // Umidade alta
    Serial.println("Umidade alta! Desativando irrigação.");
    digitalWrite(RELAY_PIN, LOW); // Desliga o relé
  }

  // Adicione condições para temperatura se necessário
  if (temperature > 30) { // Temperatura alta
    Serial.println("Temperatura alta! Aumentando irrigação.");
    digitalWrite(RELAY_PIN, HIGH);
  } else if (temperature < 20) { // Temperatura baixa
    Serial.println("Temperatura baixa! Desativando irrigação.");
    digitalWrite(RELAY_PIN, LOW);
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
    delay(1000); // Buzzer ligado por 1 segundo
    digitalWrite(BUZZER_PIN, LOW); // Desativa o buzzer
  } else {
    Serial.println("Nenhum movimento.");
  }
}