#define BLYNK_TEMPLATE_ID "TMPL6ZN0QdfoU" //Blynk Template ID
#define BLYNK_TEMPLATE_NAME "Wifi Car"    //Blynk Template Name

#define BLYNK_PRINT Serial
#include <WiFi.h>               // Library WiFi untuk ESP32
#include <BlynkSimpleEsp32.h>    // Library Blynk untuk ESP32
#include <NewPing.h>             // Library Ultrasonic Sensor

// Motor PINs (sesuaikan dengan GPIO ESP32)
#define ENA 18    // Pin Enable untuk Motor A
#define IN1 2     // Pin Input 1 untuk Motor A
#define IN2 4     // Pin Input 2 untuk Motor A
#define IN3 16    // Pin Input 1 untuk Motor B
#define IN4 17    // Pin Input 2 untuk Motor B
#define ENB 5     // Pin Enable untuk Motor B

// Ultrasonic Sensor Pin
#define TRIGGER_PIN 23  // Pin trigger sensor ultrasonik
#define ECHO_PIN 22     // Pin echo sensor ultrasonik
#define MAX_DISTANCE 200 // Maksimal jarak deteksi dalam cm

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  // Inisialisasi sensor ultrasonik

// Variabel untuk kontrol arah motor
bool forward = 0;
bool backward = 0;
bool left = 0;
bool right = 0;
int Speed = 255;  // Nilai default kecepatan motor (0 - 255)

char auth[] = "HeR-Ws1udySY-CTBeywQ6yQ8SGHp63hl"; // Masukkan Blynk Auth Token
char ssid[] = "wifiku";                           // Masukkan SSID WiFi
char pass[] = "mbotenngertos";                    // Masukkan Password WiFi

// Mode untuk pengaturan otomatis
bool autoMode = false; // Default mode manual
bool intelegentMode = false; // Default mode manual

unsigned long previousMillis = 0; // Untuk mengontrol durasi belokan
const long interval = 500;        // Durasi belokan (500ms atau 0.5 detik untuk belok 90 derajat)

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  Blynk.begin(auth, ssid, pass);
}

// Fungsi untuk mengontrol mode otomatis/manual dari Blynk

// Fungsi untuk membaca input dari Blynk (manual control)
BLYNK_WRITE(V0) { forward = param.asInt(); }
BLYNK_WRITE(V1) { backward = param.asInt(); }
BLYNK_WRITE(V2) { left = param.asInt(); }
BLYNK_WRITE(V3) { right = param.asInt(); }
BLYNK_WRITE(V4) { Speed = param.asInt(); }

BLYNK_WRITE(V5) {
  autoMode = param.asInt();  // Baca nilai dari tombol switch (1 = otomatis, 0 = manual)
}

void smartcar() {
  if (autoMode) {
    // Mode otomatis: Menghindari rintangan menggunakan sensor ultrasonik
    int distance = sonar.ping_cm();

    // Jika ada halangan dalam jarak kurang dari 20 cm, berbelok ke kanan
    if (distance > 0 && distance < 20) {
      unsigned long currentMillis = millis();

      // Belok kanan selama 500ms (90 derajat)
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        carturnleft();
        delay(300); // Mundur Sekitan 0.3 detik
        carbackward();  // Belok kanan
        delay(500);      // Delay agar belok sempurna (0.5 detik)
        carStop();       // Berhenti setelah belok
      }
    } else {
      carturnright();  // Tidak ada rintangan, maju terus
    }
  } else {
    // Mode manual: Kontrol dari Blynk
    if (forward == 1) {
      carforward();
      Serial.println("left");
    } else if (backward == 1) {
      carbackward();
      Serial.println("Right");
    } else if (left == 1) {
      carturnleft();
      Serial.println("backward");
    } else if (right == 1) {
      carturnright();
      Serial.println("forward");
    } else if (forward == 0 && backward == 0 && left == 0 && right == 0) {
      carStop();
      Serial.println("carstop");
    }
  }
}

void loop() {
  Blynk.run();
  smartcar();
}

// Fungsi motor tetap sama seperti sebelumnya:
void carforward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carbackward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carturnleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carturnright() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}