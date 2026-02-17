#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// PINES
int pin_rx= 4, pin_tx= 3; // Pines de arduino para el modulo
int sal= 8;       // Salida en pin 8


// GPS Y COMUNICACION SERIAL
TinyGPSPlus gps;
SoftwareSerial pinesModulo(pin_rx, pin_tx);

// CENTRO GEOCERCA
const double cen_lat= ;  // Latitud Centro
const double cen_lon= ; // Longitud Centro
const double rad_m= 20;   // Radio en metros

void setup() {
  Serial.begin(115200); 
  pinesModulo.begin(9600); //Baudios modulo GPS    

  pinMode(sal, OUTPUT);
  digitalWrite(sal, LOW); // salida en bajo inicialmente

  Serial.println(F("Geocerca"));
  Serial.print(F("Radio: "));
  Serial.print(rad_m);
  Serial.println(F(" metros"));
}

void loop() {
  // LECTURA MODULO GPS
  while (pinesModulo.available() > 0) {
    if (gps.encode(pinesModulo.read())) {
      Cerca();
    }
  }

  
}

void Cerca() {
  if (gps.location.isValid()) {
    // UBICACION ACTUAL
    double Lat_act = gps.location.lat();
    double Lon_act = gps.location.lng();

    //DISTANCIA ACTUAL RESPECTO AL CENTRO
    double Distancia = TinyGPSPlus::distanceBetween(Lat_act, Lon_act, cen_lat, cen_lon);

    Serial.print("Distancia: ");
    Serial.print(Distancia);
    Serial.println(" metros");

    // ALERTA 
    if (Distancia>rad_m) {
      // FUERA DEL RADIO
      Serial.println("ESTADO: FUERA DE LA GEOCERCA");
      digitalWrite(sal, HIGH); // Salida en alto
    } else {
      // DENTRO DEL RADIO
      Serial.println("ESTADO: DENTRO DE LA GEOCERCA");
      digitalWrite(sal, LOW);  //Salida en bajo
    }
    
  } 
}