// ============================================================
//  Simulación de Bolsa de Aire con MPU6050
//  Librería: MPU6050 by Electronic Cats
//  Arduino UNO: SDA=A4, SCL=A5
// ============================================================

#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu;

// --- Parámetros del algoritmo ---
const float UMBRAL_G      = 0.8;   // G's mínimos para activar
const float DELTA_V_MIN   = 0.1;   // m/s de cambio brusco mínimo
const unsigned long COOLDOWN = 3000; // ms entre activaciones

// --- Variables de estado ---
float velX = 0, velY = 0, velZ = 0;
float velX_prev = 0, velY_prev = 0, velZ_prev = 0;
unsigned long tiempoAnterior = 0;
unsigned long ultimaActivacion = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("============================================");
  Serial.println("  SISTEMA DE BOLSA DE AIRE - MPU6050");
  Serial.println("============================================");
  Serial.println("Inicializando sensor...");

  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("ERROR: No se encontro el MPU6050.");
    while (1) delay(500);
  }

  // Rango acelerómetro: ±8G  (0=2G, 1=4G, 2=8G, 3=16G)
  mpu.setFullScaleAccelRange(2);

  Serial.println("Sensor listo.");
  Serial.print("Umbral: "); Serial.print(UMBRAL_G); Serial.println(" G's");
  Serial.print("Delta-V minimo: "); Serial.print(DELTA_V_MIN); Serial.println(" m/s");
  Serial.println("--------------------------------------------");
  Serial.println("Monitoreando...");
  Serial.println();

  tiempoAnterior = millis();
}

void loop() {
  int16_t ax_raw, ay_raw, az_raw, gx, gy, gz;
  mpu.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx, &gy, &gz);

  // Convertir a m/s² (rango ±8G → sensibilidad 4096 LSB/G)
  float ax = (ax_raw / 4096.0) * 9.81;
  float ay = (ay_raw / 4096.0) * 9.81;
  float az = (az_raw / 4096.0) * 9.81 - 9.81; // restar gravedad

  unsigned long ahora = millis();
  float dt = (ahora - tiempoAnterior) / 1000.0;
  tiempoAnterior = ahora;

  // Magnitud de aceleración en G's
  float mag_ms2 = sqrt(ax*ax + ay*ay + az*az);
  float mag_g   = mag_ms2 / 9.81;

  // Integración → velocidad y delta-V
  velX_prev = velX; velY_prev = velY; velZ_prev = velZ;
  velX += ax * dt;
  velY += ay * dt;
  velZ += az * dt;

  float deltaV = sqrt(
    pow(velX - velX_prev, 2) +
    pow(velY - velY_prev, 2) +
    pow(velZ - velZ_prev, 2)
  );

  // --- Detección de impacto ---
  bool impacto       = (mag_g >= UMBRAL_G) && (deltaV >= DELTA_V_MIN);
  bool fueraCooldown = (ahora - ultimaActivacion) > COOLDOWN;

  if (impacto && fueraCooldown) {
    activarAirbag(mag_g, deltaV, ax, ay, az);
    ultimaActivacion = ahora;
    velX = velY = velZ = 0; // reset velocidad
  }

  // Log cada 100 ms
  static unsigned long ultimoLog = 0;
  if (ahora - ultimoLog >= 100) {
    Serial.print("G: ");    Serial.print(mag_g, 2);
    Serial.print(" | dV: "); Serial.print(deltaV, 3);
    Serial.print(" m/s | AX:"); Serial.print(ax, 1);
    Serial.print(" AY:");    Serial.print(ay, 1);
    Serial.print(" AZ:");    Serial.println(az, 1);
    ultimoLog = ahora;
  }

  delay(10);
}

// ============================================================
void activarAirbag(float g, float dv, float ax, float ay, float az) {
  Serial.println();
  Serial.println("############################################");
  Serial.println("#                                          #");
  Serial.println("#      BOLSA DE AIRE ACTIVADA !!!          #");
  Serial.println("#                                          #");
  Serial.println("############################################");
  Serial.println();
  Serial.println("--- DATOS DEL IMPACTO ---");
  Serial.print("  Aceleracion pico : "); Serial.print(g, 2);  Serial.println(" G's");
  Serial.print("  Delta-V (dV)     : "); Serial.print(dv, 3); Serial.println(" m/s");
  Serial.print("  Eje dominante    : ");
  float absX = abs(ax), absY = abs(ay), absZ = abs(az);
  if      (absX >= absY && absX >= absZ) Serial.println("X (lateral)");
  else if (absY >= absX && absY >= absZ) Serial.println("Y (frontal)");
  else                                   Serial.println("Z (vertical)");
  Serial.print("  Tiempo           : "); Serial.print(millis()/1000.0, 2); Serial.println(" s");
  Serial.println("-------------------------");
  Serial.println("Cooldown 3 segundos...");
  Serial.println();
}
