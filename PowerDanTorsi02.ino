//DYNOTEST DENGAN MEMPERHITUNGKAN BERAT KENDARAAN DAN BERAT PENGEMUDI
// Konstanta
const int pinEncoderRoller = 2; // Pin untuk input encoder roller (interrupt 0)
const int pinEngineRPM = 3;     // Pin untuk input RPM engine (interrupt 1)

volatile unsigned long pulseCountRoller = 0;
volatile unsigned long pulseCountEngine = 0;

unsigned long previousMillis = 0;
const unsigned long interval = 1000; // Interval pengukuran dalam ms (1 detik)

// Konstanta untuk menghitung daya
float rollerMass = 200;         // Massa roller (kg)
float rollerRadius = 0.5;       // Radius roller (meter)
float vehicleMass = 150;        // Massa kendaraan (kg)
float driverMass = 70;          // Massa pengemudi (kg)
float wheelRadius = 0.3;        // Radius roda kendaraan (meter)
float inertiaTotal;             // Momen inersia total (roller + kendaraan)

float previousAngularVelocityRoller = 0; // Kecepatan sudut roller sebelumnya
float deltaEnergy = 0; // Delta energi rotasi
float power = 0;       // Daya dalam watt
float torque = 0;      // Torsi dalam Nm

void setup() {
  Serial.begin(9600);

  // Setup pin input untuk encoder roller dan sinyal RPM mesin
  pinMode(pinEncoderRoller, INPUT);
  pinMode(pinEngineRPM, INPUT);
  
  // Attach interrupt untuk menangkap pulsa dari encoder roller
  attachInterrupt(digitalPinToInterrupt(pinEncoderRoller), countPulseRoller, RISING);
  attachInterrupt(digitalPinToInterrupt(pinEngineRPM), countPulseEngine, RISING);
  
  // Menghitung momen inersia roller
  float inertiaRoller = 0.5 * rollerMass * pow(rollerRadius, 2);
  
  // Menghitung momen inersia total (roller + kendaraan)
  inertiaTotal = inertiaRoller + (vehicleMass + driverMass) * pow(wheelRadius, 2);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // Interval 1 detik
    previousMillis = currentMillis;
    
    // Hitung RPM roller berdasarkan jumlah pulsa
    float rpmRoller = (pulseCountRoller * 60.0) / 20.0; // Asumsikan 20 pulsa per revolusi (sesuaikan dengan encoder)
    pulseCountRoller = 0; // Reset pulse count setelah pengukuran

    // Hitung RPM engine berdasarkan jumlah pulsa
    float rpmEngine = (pulseCountEngine * 60.0) / 1.0; // Sesuaikan dengan sinyal engine
    pulseCountEngine = 0; // Reset pulse count setelah pengukuran

    // Menghitung kecepatan sudut (angular velocity) dari roller (rad/s)
    float angularVelocityRoller = (2 * PI * rpmRoller) / 60.0;

    // Menghitung energi rotasi dengan momen inersia total
    float energyRoller = 0.5 * inertiaTotal * pow(angularVelocityRoller, 2);

    // Menghitung delta energi rotasi
    deltaEnergy = energyRoller - (0.5 * inertiaTotal * pow(previousAngularVelocityRoller, 2));

    // Hitung power (daya) dalam watt
    power = deltaEnergy; // Delta energi per detik (interval 1 detik)
    
    // Hitung torsi berdasarkan daya dan RPM engine
    float angularVelocityEngine = (2 * PI * rpmEngine) / 60.0;
    torque = power / angularVelocityEngine;

    // Simpan kecepatan sudut sebelumnya
    previousAngularVelocityRoller = angularVelocityRoller;

    // Tampilkan hasil pada Serial Monitor
    Serial.print("RPM Roller: ");
    Serial.print(rpmRoller);
    Serial.print(" | RPM Engine: ");
    Serial.print(rpmEngine);
    Serial.print(" | Power (W): ");
    Serial.print(power);
    Serial.print(" | Torque (Nm): ");
    Serial.println(torque);
  }
}

void countPulseRoller() {
  pulseCountRoller++;
}

void countPulseEngine() {
  pulseCountEngine++;
}
