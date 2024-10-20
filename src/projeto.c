#include <DHT.h>

#define DHTPIN 23        // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22   // Tipo do sensor DHT
#define RELAY_PIN 5     // Pino do relé

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Começa com o relé desligado
}

void loop() {
  delay(2000); // Atraso para evitar leituras muito frequentes

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