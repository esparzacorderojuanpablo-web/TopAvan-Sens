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
const int ENA = 5;  // Control de velocidad (PWM)
const int IN1 = 6;  // Dirección del motor
const int IN2 = 7;  // Dirección del motor

// Ángulos del Servo para Etapas
const int ANGULO_SIN_RIESGO = 0;
const int ANGULO_EN_RIESGO  = 90;
const int ANGULO_SOS        = 180;

// Umbral del MPU 
// Ajusta estos valores según las pruebas en tu mesa
const long UMBRAL_RIESGO = 20000; 
const long UMBRAL_SOS    = 30000;

// Umbrales de Altitud (Metros) para el motor
const float ALTITUD_MIN_MOTOR = 2230.0;  
const float ALTITUD_MAX_MOTOR = 2400.0; 

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
  //- Lectura del MPU6050 y calculo de magnitud
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

  //- Control del servo para etapas de riesgo
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

  //---Leer BMP280 y control del motor
  float altitud = bmp.readAltitude(1013.25); 
  int velocidadMotor = 0;

  if (altitud >= ALTITUD_MIN_MOTOR) {
    // Limitamos el PWM del motor
    float altitudTop = constrain(altitud, ALTITUD_MIN_MOTOR, ALTITUD_MAX_MOTOR);
    
    // Mapeamos proporcionalmente: a menor altitud -> 60 PWM, a mayor altitud -> 255 PWM
    velocidadMotor = map(altitudTop, ALTITUD_MIN_MOTOR, ALTITUD_MAX_MOTOR, 60, 255);
    
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

  //- Transmitir Datos por Bluetooth
  BT.print("Estado: ");  BT.print(estadoActual);
  BT.print(" | Altitud (mar): "); BT.print(altitud, 1); BT.print("m");
  BT.print(" | Abertura Tanque: "); BT.print(map(velocidadMotor, 0, 255, 0, 100)); BT.println("%");
  
  //Monitor Serial
  Serial.print("Mag MPU: "); Serial.print(magnitud);
  Serial.print(" | Estado: "); Serial.print(estadoActual);
  Serial.print(" | Altitud: "); Serial.print(altitud, 1);
  Serial.print("m | Motor PWM: "); Serial.println(velocidadMotor);
  
  delay(600); 
}
