// ============================================================
//  Medición de Fuerza de Empujón con MPU6050
//  Librería: MPU6050 by Electronic Cats
NEW SKETCH

//  Sensor montado en el objeto empujado
NEW SKETCH

//  Masa del objeto: 50g = 0.05 kg
//  Rango de distancia válida: 3 cm a 10 cm
// ============================================================

#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu;

// --- Configuración física ---
const float MASA_KG       = 0.05;   // 50 gramos en kg
const float DIST_MIN_M    = 0.03;   // 3 cm en metros
const float DIST_MAX_M    = 0.10;   // 10 cm en metros

// --- Umbrales de detección ---
const float UMBRAL_INICIO_MS2 = 2.0;  // m/s² mínimo para detectar inicio del empujón
const float UMBRAL_FIN_MS2    = 1.0;  // m/s² al que se considera que el empujón terminó
const unsigned long TIEMPO_MIN_MS = 30;   // duración mínima del empujón (ms)
const unsigned long TIEMPO_MAX_MS = 800;  // duración máxima del empujón (ms)

// --- Variables de estado ---
enum Estado { REPOSO, EMPUJANDO };
Estado estadoActual = REPOSO;

float velX = 0, velY = 0, velZ = 0;
float distX = 0, distY = 0, distZ = 0;
float acumFuerza = 0;
int   muestrasFuerza = 0;

unsigned long tiempoAnterior  = 0;
unsigned long tiempoInicioEmpujon = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("============================================");
  Serial.println("  MEDICION DE FUERZA DE EMPUJON - MPU6050");
  Serial.println("============================================");
  Serial.println("Inicializando sensor...");

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("ERROR: No se encontro el MPU6050.");
    while (1) delay(500);
  }

  // Rango ±4G para mayor precisión en empujones suaves (sensibilidad: 8192 LSB/G)
  mpu.setFullScaleAccelRange(1);

  Serial.println("Sensor listo.");
  Serial.println("Masa del objeto : 50 g");
  Serial.println("Distancia valida: 3 cm a 10 cm");
  Serial.println("--------------------------------------------");
  Serial.println("Coloca el objeto en reposo y empuja...");
  Serial.println();

  tiempoAnterior = millis();
}

void loop() {
  int16_t ax_raw, ay_raw, az_raw, gx, gy, gz;
  mpu.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx, &gy, &gz);

  // Convertir a m/s² (rango ±4G → sensibilidad 8192 LSB/G)
  float ax = (ax_raw / 8192.0) * 9.81;
  float ay = (ay_raw / 8192.0) * 9.81;
  float az = (az_raw / 8192.0) * 9.81 - 9.81; // restar gravedad

  unsigned long ahora = millis();
  float dt = (ahora - tiempoAnterior) / 1000.0;
  tiempoAnterior = ahora;

  // Magnitud de aceleración neta (sin gravedad)
  float mag = sqrt(ax*ax + ay*ay + az*az);

  // Fuerza instantánea F = m * a
  float fuerzaInst = MASA_KG * mag;

  // ============================================================
  //  MÁQUINA DE ESTADOS
  // ============================================================
  switch (estadoActual) {

    // ── REPOSO: esperando inicio de empujón ──
    case REPOSO:
      if (mag >= UMBRAL_INICIO_MS2) {
        // Inicio detectado
        estadoActual       = EMPUJANDO;
        tiempoInicioEmpujon = ahora;

        // Reset acumuladores
        velX = velY = velZ = 0;
        distX = distY = distZ = 0;
        acumFuerza    = 0;
        muestrasFuerza = 0;

        Serial.println(">> Empujon detectado, midiendo...");
      }
      break;

    // ── EMPUJANDO: acumulando datos ──
    case EMPUJANDO:
      // Integrar aceleración → velocidad
      velX += ax * dt;
      velY += ay * dt;
      velZ += az * dt;

      // Integrar velocidad → distancia
      distX += velX * dt;
      distY += velY * dt;
      distZ += velZ * dt;

      // Acumular fuerza para promedio
      acumFuerza += fuerzaInst;
      muestrasFuerza++;

      // Detectar fin del empujón (aceleración baja de nuevo)
      if (mag <= UMBRAL_FIN_MS2) {
        unsigned long duracion = ahora - tiempoInicioEmpujon;

        // Distancia total recorrida
        float distTotal = sqrt(distX*distX + distY*distY + distZ*distZ);

        // Fuerza promedio durante el empujón
        float fuerzaPromedio = (muestrasFuerza > 0) ? (acumFuerza / muestrasFuerza) : 0;

        // Fuerza pico (última medición antes de caer, aproximada como la más alta acumulada)
        // Se calcula como F = m * a con la magnitud en el instante de mayor aceleración
        // Para simplicidad usamos el promedio y la notamos como estimación
        bool duracionValida = (duracion >= TIEMPO_MIN_MS && duracion <= TIEMPO_MAX_MS);
        bool distanciaValida = (distTotal >= DIST_MIN_M && distTotal <= DIST_MAX_M);

        reportar(fuerzaPromedio, distTotal, duracion, duracionValida, distanciaValida);

        estadoActual = REPOSO;
      }

      // Seguridad: si dura demasiado, cancelar
      if ((ahora - tiempoInicioEmpujon) > TIEMPO_MAX_MS) {
        Serial.println(">> Empujon demasiado largo, descartado.");
        Serial.println();
        estadoActual = REPOSO;
      }
      break;
  }

  delay(5); // muestreo cada 5ms = 200Hz
}

// ============================================================
//  Reporte en Serial Monitor
// ============================================================
void reportar(float fuerza, float distancia, unsigned long duracion, bool durOk, bool distOk) {
  Serial.println("--------------------------------------------");
  Serial.println("         RESULTADO DEL EMPUJON");
  Serial.println("--------------------------------------------");

  Serial.print("  Fuerza promedio : ");
  Serial.print(fuerza * 1000, 2); // en mili-Newtons para mayor resolución
  Serial.println(" mN");

  Serial.print("  Fuerza promedio : ");
  Serial.print(fuerza, 5);
  Serial.println(" N");

  Serial.print("  Distancia est.  : ");
  Serial.print(distancia * 100, 2); // convertir a cm
  Serial.println(" cm");

  Serial.print("  Duracion        : ");
  Serial.print(duracion);
  Serial.println(" ms");

  Serial.print("  Distancia valida: ");
  Serial.println(distOk ? "SI (3-10 cm)" : "NO (fuera de rango)");

  Serial.print("  Duracion valida : ");
  Serial.println(durOk ? "SI" : "NO (muy corto o muy largo)");

  if (distOk && durOk) {
    Serial.println();
    Serial.println("  ✓ MEDICION DENTRO DEL RANGO ESPERADO");
  } else {
    Serial.println();
    Serial.println("  ✗ Medicion fuera de rango, repite el empujon.");
  }

  Serial.println("--------------------------------------------");
  Serial.println();
}