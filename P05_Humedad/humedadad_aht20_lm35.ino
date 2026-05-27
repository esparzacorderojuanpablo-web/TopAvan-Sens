#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

int pinLM35 = A0;       // Pin analógico del LM35
int pinLM35_2 = A1;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL); // Referencia interna 1.1V para el LM35
  Wire.begin();
  delay(1000);

  Serial.println("Iniciando sensores AHT20 + BMP280 + LM35...");

  if (!bmp.begin(0x77)) {
    Serial.println("No se encontro el sensor BMP280");
    while (1);
  }

  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );

  if (!aht.begin()) {
    Serial.println("No se encontro el sensor AHT20");
    while (1);
  }

  Serial.println("Sensores listos");
  Serial.println("-----------------------------");
}

void loop() {
  // --- LM35 ---
  float lecturaLM35 = analogRead(pinLM35);
  float tempLM35 = (1.1 * lecturaLM35 * 100.0) / 1024.0;
  float lecturaLM352 = analogRead(pinLM35_2);
  float tempLM352 = (1.1 * lecturaLM352 * 100.0) / 1024.0;
  // --- AHT20 ---
  sensors_event_t humedad, temperatura;
  aht.getEvent(&humedad, &temperatura);
  float temperaturaC   = temperatura.temperature;
  float temperaturaK   = temperaturaC + 273.15;
  float humedadRelativa = humedad.relative_humidity;

  // --- Mostrar resultados ---
  Serial.println("=== LM35 ===");
  Serial.print("Temperatura LM35: ");
  Serial.print(tempLM35, 1);
  Serial.println(" °C");
    Serial.println("=== LM35_2 ===");
  Serial.print("Temperatura LM35: ");
  Serial.print(tempLM352, 1);
  Serial.println(" °C");
  Serial.println("=== AHT20 ===");
  Serial.print("Temperatura:      ");
  Serial.print(temperaturaC, 1);
  Serial.println(" °C");
  Serial.print("Temperatura K:    ");
  Serial.print(temperaturaK, 1);
  Serial.println(" K");
  Serial.print("Humedad relativa: ");
  Serial.print(humedadRelativa, 1);
  Serial.println(" %");
  Serial.println("-----------------------------");
  delay(2000);
}
