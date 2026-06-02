const int sensorGas = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {

  long suma = 0;

  for(int i=0; i<50; i++) {
    suma += analogRead(sensorGas);
    delay(20);
  }

  float promedio = suma / 50.0;

  float voltaje = promedio * (5.0 / 1023.0);

  Serial.print("Promedio ADC: ");
  Serial.print(promedio);

  Serial.print("  Voltaje: ");
  Serial.print(voltaje);
  Serial.println(" V");

  delay(1000);
}
