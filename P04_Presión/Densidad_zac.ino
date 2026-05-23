#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

// Constante específica del aire seco
const float R = 287.05; // J/(kg·K)

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Serial.println("Iniciando sensores AHT20 + BMP280...");

  // Inicializar AHT20
  if (!aht.begin()) {
    Serial.println("No se encontro el sensor AHT20");
    while (1);
  }

  // Inicializar BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("No se encontro BMP280 en 0x76, probando 0x77...");

    if (!bmp.begin(0x77)) {
      Serial.println("No se encontro el sensor BMP280");
      while (1);
    }
  }

  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );

  Serial.println("Sensores listos");
  Serial.println("-----------------------------");
}

void loop() {
  sensors_event_t humedad, temperatura;

  // Leer temperatura del AHT20
  aht.getEvent(&humedad, &temperatura);

  float temperaturaC = temperatura.temperature; // °C
  float temperaturaK = temperaturaC + 273.15;   // K

  // Leer presión del BMP280
  float presionPa = bmp.readPressure(); // Pa

  // Calcular densidad del aire
  float densidadAire = presionPa / (R * temperaturaK);

  // Mostrar resultados
  Serial.print("Temperatura: ");
  Serial.print(temperaturaC);
  Serial.println(" °C");

  Serial.print("Temperatura Kelvin: ");
  Serial.print(temperaturaK);
  Serial.println(" K");

  Serial.print("Presion: ");
  Serial.print(presionPa);
  Serial.println(" Pa");

  Serial.print("Densidad del aire: ");
  Serial.print(densidadAire, 4);
  Serial.println(" kg/m^3");

  Serial.println("-----------------------------");

  delay(1000);
}
