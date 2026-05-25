#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

// Humedad relativa fija (%)
const float HR = 43.0;

// Calcula la presion de vapor de agua saturada (Pa) - formula de Magnus
float presionVaporSaturado(float T) {
  // T en Celsius
  return 611.657 * exp(17.2799 * T / (238.3 + T));
}

// Calcula la densidad del aire segun CIPM-2007 (kg/m3)
float densidadAireCIPM2007(float T_C, float P_Pa, float h) {
  float T = T_C + 273.15;  // Kelvin

  // Presion parcial de vapor de agua
  float Psv = presionVaporSaturado(T_C);  // Pa
  float Pv  = (h / 100.0) * Psv;         // Pa

  // Fraccion molar de vapor de agua
  float xv = Pv / P_Pa;

  // Fraccion molar de CO2 (valor estandar CIPM: 0.0004)
  float xc = 0.0004;

  // Masa molar del aire humedo (kg/mol)
  float Ma = 1e-3 * (28.96546 + 12.011 * (xc - 0.0004) - 10.8902 * xv);

  // Factor de compresibilidad Z 
  float Z = 1.0
    - (P_Pa / T) * (1.58123e-6
    - 2.9331e-8  * T_C
    + 1.1043e-10 * T_C * T_C
    + (5.707e-6  - 2.051e-8  * T_C) * xv)
    + (P_Pa / T) * (P_Pa / T) * 1.9898e-4;

  // Constante universal de los gases
  const float R = 8.314462;  // J/(mol·K)

  // Densidad (kg/m3)
  float densidad = (P_Pa * Ma) / (Z * R * T);

  return densidad;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!bmp.begin(0x77)) {
    Serial.println(F("No se encontro BMP280"));
    while (1);
  }

  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );

  Serial.println(F("BMP280 listo - Calculo de densidad CIPM-2007"));
  Serial.print(F("Humedad relativa fija: "));
  Serial.print(HR);
  Serial.println(F(" %"));
  Serial.println();
}

void loop() {
  float T = bmp.readTemperature();  // Celsius
  float P = bmp.readPressure();     // Pa

  float densidad = densidadAireCIPM2007(T, P, HR);

  Serial.println(F("--- Medicion ---"));

  Serial.print(F("Temperatura : "));
  Serial.print(T, 2);
  Serial.println(F(" C"));

  Serial.print(F("Presion     : "));
  Serial.print(P / 100.0, 2);
  Serial.println(F(" hPa"));

  Serial.print(F("Hum. relat. : "));
  Serial.print(HR, 1);
  Serial.println(F(" %"));

  Serial.print(F("Densidad    : "));
  Serial.print(densidad, 6);
  Serial.println(F(" kg/m3"));

  Serial.println();
  delay(2000);
}
