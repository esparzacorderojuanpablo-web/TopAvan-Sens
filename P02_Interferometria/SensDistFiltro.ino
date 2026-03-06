#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//Caracteristicas filtro
float alpha = 0.25;              
float d_filtro = 0.0;  
bool ini = false;       //Para la primera lectura del sensor

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL53L0X test");

  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }

  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {  
    float d_actual = measure.RangeMilliMeter-26;

    //Inicio del filtro
    if (!ini) {
      d_filtro = d_actual;
      ini = true;
    } else {
      //Filtrado de datos
      d_filtro=alpha*d_actual+(1-alpha)*d_filtro;
    }

    Serial.print("Distancia cruda (mm): ");
    Serial.print(d_actual);

    Serial.print("  |  Distancia filtrada (mm): ");
    Serial.println(d_filtro);

  } else {
    Serial.println("Out of range");
  }

  delay(100);
}
