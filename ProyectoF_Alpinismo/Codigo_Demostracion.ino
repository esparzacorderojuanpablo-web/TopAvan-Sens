#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>
#include <Servo.h>

// Configuración de pines
SoftwareSerial BT(2, 3); 
Adafruit_BMP280 bmp;
Servo servoRiesgo;

// MPU6050
const int MPU_addr = 0x68; 
int16_t AcX, AcY, AcZ;

// Puente H 
const int ENA = 5;  
const int IN1 = 6;  
const int IN2 = 7;  

// Ángulos del Servo para Etapas
const int ANGULO_SIN_RIESGO = 0;
const int ANGULO_EN_RIESGO  = 90;
const int ANGULO_SOS        = 180;

// Umbral del MPU 
const long UMBRAL_RIESGO = 19000; 
const long UMBRAL_SOS    = 30000;

// Umbrales de Altitud (Metros) para el motor
float ALTITUD_MIN_MOTOR = 2230.0;  
const float ALTITUD_MAX_MOTOR = 2650.0; 

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  
  Serial.begin(9600);
  BT.begin(9600);

  // Inicializar Servo
  servoRiesgo.attach(9);
  servoRiesgo.write(ANGULO_SIN_RIESGO);

  // Inicializar Pines del Motor
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Motor inicialmente apagado
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  // Inicializar MPU6050
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  // Inicializar BMP280
  if (!bmp.begin(0x77) && !bmp.begin(0x76)) {
    Serial.println("Error BMP280");
  }
  
  Serial.println("Sistema de Seguridad Activado.");
}

void loop() {
  // Control de umbral minimo por bluetooth para demostración
  if (BT.available() > 0 || Serial.available() > 0) {
    char comando = (BT.available() > 0) ? BT.read() : Serial.read();
    
    if (comando == 'U' || comando == 'u') {
      ALTITUD_MIN_MOTOR = 2400.0; 
      BT.println("\n[UMBRAL SUBIÓ A 2400m ");
      Serial.println("\n[UMBRAL SUBIÓ A 2400m ");
    }
    else if (comando == 'D' || comando == 'd') {
      ALTITUD_MIN_MOTOR = 2100.0; 
      BT.println("\n[UMBRAL BAJÓ A 2100m ");
      Serial.println("\n[UMBRAL BAJÓ A 2100m ");
    }
    else if (comando == 'R' || comando == 'r') {
      ALTITUD_MIN_MOTOR = 2230.0; 
      BT.println("\n[UMBRAL RECONFIGURADO A VALOR ORIGINAL: 2230m]");
      Serial.println("\n[UMBRAL RECONFIGURADO A VALOR ORIGINAL: 2230m]");
    }
  }

  //Lectura de datos del MPU6050 y magnitud
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);
  
  if(Wire.available() >= 6) {
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
  }

  // Magnitud de la aceleración
  long magnitud = sqrt(pow(AcX, 2) + pow(AcY, 2) + pow(AcZ, 2));

  //Control del servo para las etapas
  String estadoActual = "";
  if (magnitud < UMBRAL_RIESGO) {
    servoRiesgo.write(ANGULO_SIN_RIESGO);
    estadoActual = "SIN RIESGO";
  } 
  else if (magnitud >= UMBRAL_RIESGO && magnitud < UMBRAL_SOS) {
    servoRiesgo.write(ANGULO_EN_RIESGO);
    estadoActual = "EN RIESGO";
  } 
  else if (magnitud >= UMBRAL_SOS) {
    servoRiesgo.write(ANGULO_SOS);
    estadoActual = "SOS / EMERGENCIA";
  }

  //Lectura de datos del BMP280 
  float altitud = bmp.readAltitude(1013.25); 
  int velocidadMotor = 0;

  if (altitud >= ALTITUD_MIN_MOTOR) {
    //Limite de altitud
    float altitudTop = constrain(altitud, ALTITUD_MIN_MOTOR, ALTITUD_MAX_MOTOR);
    
    // Mapeo de la velocidad del motor 
    velocidadMotor = map(altitudTop, ALTITUD_MIN_MOTOR, ALTITUD_MAX_MOTOR, 140, 255);
    
    // Dirección de giro del motor
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, velocidadMotor);
  } else {
    // Apagar motor 
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }

  //Transmisión de datos por bluetooth
  BT.print("Estado: ");  BT.print(estadoActual);
  BT.print(" | Umbral Min: "); BT.print(ALTITUD_MIN_MOTOR, 0); BT.print("m");
  BT.print(" | Altitud actual: "); BT.print(altitud, 1); BT.print("m");
  BT.print(" | Abertura Tanque: "); BT.print(map(velocidadMotor, 0, 255, 0, 100)); BT.println("%");
  
  //Monitor Serial
  Serial.print("Mag MPU: "); Serial.print(magnitud);
  Serial.print(" | Umbral Min: "); Serial.print(ALTITUD_MIN_MOTOR, 0);
  Serial.print("m | Altitud: "); Serial.print(altitud, 1);
  Serial.print("m | Motor PWM: "); Serial.println(velocidadMotor);
  
  delay(600); 
}
