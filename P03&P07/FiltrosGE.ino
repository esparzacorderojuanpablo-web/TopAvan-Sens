
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

MPU6050 sensor;

int ax, ay, az;
int gx, gy, gz;

long tiempo_prev;
float dt;
float ang_x, ang_y; 
float ang_x_prev, ang_y_prev;

void setup() {
  Serial.begin(115200);    
  Wire.begin();             
  sensor.initialize();    

  if (sensor.testConnection()) Serial.println("Sensor iniciado correctamente");
  else Serial.println("Error al iniciar el sensor");
}

void loop() {
  
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  
  dt = (millis()-tiempo_prev)/1000.0;
  tiempo_prev=millis();
  
  //ANGULOS ACELEROMETRO
  float accel_ang_x=atan(ay/sqrt(pow(ax,2) + pow(az,2)))*(180.0/3.14);
  float accel_ang_y=atan(ax/sqrt(pow(ay,2) + pow(az,2)))*(180.0/3.14);
  float accel_ang_z=atan(az/sqrt(pow(ay,2) + pow(ax,2)))*(180.0/3.14);  

  //ANGULOS GIROSCOPIO  
  ang_x = 0.98*(ang_x_prev+(gx/131)*dt) + 0.02*accel_ang_x;
  ang_y = 0.98*(ang_y_prev+(gy/131)*dt) + 0.02*accel_ang_y;
  //ang_z = ang_z_prev + 0.02*accel_ang_z;
  
  
  ang_x_prev=ang_x;
  ang_y_prev=ang_y;
  //ang_z_prev=ang_z;

Serial.print("aX: ");
Serial.print(ax); 
Serial.print("\taY: ");
Serial.println(ay);
Serial.print("\taZ: ");
Serial.println(az);

  delay(10);
}
