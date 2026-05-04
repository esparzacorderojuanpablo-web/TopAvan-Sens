#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 mpu;

int16_t ax, ay, az;
int estado = 0;
bool estado_boton = false;
const int boton = 2; //Pin digital

void setup() {
  Serial.begin(115200);    
  Wire.begin();             
  mpu.initialize();    
  pinMode(boton, INPUT_PULLUP);

  if (mpu.testConnection()) 
  Serial.println("Iniciado correctamente.\n");
  else 
  Serial.println("Error al iniciar.\n");
}

void loop() {
  bool leerBoton = digitalRead(boton);
  if (leerBoton == LOW && !estado_boton) {
    estado = !estado;       
    estado_boton = true;  
    delay(50);               
  }
  
  if (leerBoton == HIGH) {
    estado_boton = false;
  }
  mpu.getAcceleration(&ax, &ay, &az);

  Serial.print(ax);
  Serial.print(","); 
  Serial.print(ay);
  Serial.print(",");
  Serial.print(az);
  Serial.print(",");

  Serial.println(estado); // Set/Reset Boton
  delay(10);
}
