#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

const int relayPin1 = 3;
const int relayPin2 = 4;
const int relayPin3 = 5;
const int ldrPin = 6;
const int controlPin = 7;

DHT dht(DHTPIN, DHTTYPE);

bool manualControl = false;
bool relay1State = HIGH;
bool relay2State = HIGH;
bool relay3State = HIGH;
float temperature = 0;

void setup() {
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(controlPin, INPUT_PULLUP); // Mengatur controlPin sebagai input dengan pull-up internal

  digitalWrite(relayPin1, HIGH); // Matikan relay pada awal
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);

  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // Baca suhu dari sensor DHT11
  float temperature = dht.readTemperature();
  
  // Periksa apakah pembacaan berhasil
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  // Jika suhu di atas ambang batas tertentu (misalnya 33 derajat Celsius), nyalakan relay (dan kipas)
  if (temperature < 33) {
    digitalWrite(relayPin1, LOW);  // Aktifkan relay pertama (dan kipas pertama)
    digitalWrite(relayPin2, LOW);  // Aktifkan relay kedua (dan kipas kedua)
  } else {
    digitalWrite(relayPin1, HIGH); // Nonaktifkan relay pertama (dan kipas pertama)
    digitalWrite(relayPin2, HIGH); // Nonaktifkan relay kedua (dan kipas kedua)
  }

  // Baca status LDR
  int ldrStatus = digitalRead(ldrPin);

  // Jika cahaya gelap, nyalakan LED
  if (ldrStatus == LOW) {  // Asumsikan LOW berarti gelap dan HIGH berarti terang
    digitalWrite(relayPin3, LOW);  // Aktifkan relay ketiga (dan nyalakan LED)
  } else {
    digitalWrite(relayPin3, HIGH); // Nonaktifkan relay ketiga (dan matikan LED)
  }

  delay(1000);  // Tunggu 1 detik sebelum membaca lagi
  
  // Baca data dari ESP8266
  if (Serial.available() > 0) {
    char command = Serial.read();
    switch (command) {
      case 'A': digitalWrite(relayPin1, LOW); break; // Hidupkan relay pertama
      case 'B': digitalWrite(relayPin1, HIGH); break; // Matikan relay pertama
      case 'C': digitalWrite(relayPin2, LOW); break; // Hidupkan relay kedua
      case 'D': digitalWrite(relayPin2, HIGH); break; // Matikan relay kedua
      case 'E': digitalWrite(relayPin3, LOW); break; // Hidupkan relay ketiga (LED)
      case 'F': digitalWrite(relayPin3, HIGH); break; // Matikan relay ketiga (LED)
    }
  }
}