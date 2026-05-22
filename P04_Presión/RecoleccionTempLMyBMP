#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

const int LM35 = A0;

float tempLM35;
float tempBMP;

unsigned long tiempo;

void setup() {

  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("BMP180 no detectado");
    while (1);
  }

  Serial.println("Tiempo(s),LM35,BMP180");
}

void loop() {

  // Tiempo en segundos
  tiempo = millis() / 1000;

  // LM35
  int lectura = analogRead(LM35);

  float voltaje = lectura * (5.0 / 1023.0);

  tempLM35 = voltaje * 100.0;

  // BMP180
  tempBMP = bmp.readTemperature();

  // Enviar datos
  Serial.print(tiempo);
  Serial.print(",");

  Serial.print(tempLM35);
  Serial.print(",");

  Serial.println(tempBMP);

  delay(1000);
}
