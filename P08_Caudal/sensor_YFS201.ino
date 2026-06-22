volatile int pulsos = 0;
float caudal_L_m = 0;
float caudal_m3_m = 0;
float volumen_L = 0;
unsigned long tiempoAnterior = 0;
const float factorK = 7.5; // Factor de calibración YF-S201

void conteoPulsos() {
  pulsos++;
}

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), conteoPulsos, FALLING);
}

void loop() {
  if (millis() - tiempoAnterior >= 1000) {
    detachInterrupt(digitalPinToInterrupt(2));
    caudal_L_m = (pulsos / factorK);
    caudal_m3_m = caudal_L_m / 1000.0;
    volumen_L += (caudal_L_m / 60.0);
    
    Serial.print("Caudal (L/min): ");
    Serial.println(caudal_L_m);
    
    pulsos = 0;
    tiempoAnterior = millis();
    attachInterrupt(digitalPinToInterrupt(2), conteoPulsos, FALLING);
  }
}
