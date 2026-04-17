#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PMS.h>

// ---------------- WIFI ----------------
#define WIFI_SSID "saroj321_fbnpa"
#define WIFI_PASSWORD "CLFE25C6E7"

// ---------------- FIREBASE ----------------
#define DATABASE_URL "https://aqi-monitor-db-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define DATABASE_SECRET "YOUR_LEGACY_TOKEN_HERE"  // IMPORTANT

FirebaseData fbdo;
FirebaseConfig config;

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- PMS ----------------
HardwareSerial pmsSerial(2);
PMS pms(pmsSerial);
PMS::DATA data;

// ---------------- PIN ----------------
const int potPin = 34;

// ---------------- DATA ----------------
int pm1_final = 0;
int pm25_final = 0;
int pm10_final = 0;

// ---------------- TIMERS ----------------
unsigned long lastFirebaseUpdate = 0;
unsigned long lastLcdUpdate = 0;

const int FIREBASE_INTERVAL = 2000;
const int LCD_INTERVAL = 250;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // PMS
  pmsSerial.begin(9600, SERIAL_8N1, 27, 26);

  // LCD
  Wire.begin(33, 32);
  lcd.begin(16, 2);
  lcd.backlight();

  lcd.print("WiFi Connecting");

  // WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  lcd.clear();
  lcd.print("WiFi OK");

  // FIREBASE (LEGACY MODE)
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.begin(&config, nullptr);
  Firebase.reconnectWiFi(true);

  delay(1500);
}

// ================= LOOP =================
void loop() {

  // PMS READ
  if (pms.read(data)) {
    pm1_final  = data.PM_AE_UG_1_0;
    pm25_final = data.PM_AE_UG_2_5;
    pm10_final = data.PM_AE_UG_10_0;
  }

  unsigned long now = millis();

  // LCD
  if (now - lastLcdUpdate >= LCD_INTERVAL) {
    lastLcdUpdate = now;

    int potValue = analogRead(potPin);

    lcd.clear();

    if (potValue < 1365) {
      lcd.setCursor(0, 0);
      lcd.print("PM1:");
      lcd.print(pm1_final);

      lcd.setCursor(0, 1);
      lcd.print("PM2.5:");
      lcd.print(pm25_final);
    }
    else if (potValue < 2730) {
      lcd.setCursor(0, 0);
      lcd.print("PM10:");
      lcd.print(pm10_final);

      lcd.setCursor(0, 1);
      lcd.print("Firebase OK");
    }
    else {
      lcd.setCursor(0, 0);
      lcd.print("AQ Monitor");
      lcd.setCursor(0, 1);
      lcd.print("Live");
    }
  }

  // FIREBASE SEND
  if (now - lastFirebaseUpdate >= FIREBASE_INTERVAL) {
    lastFirebaseUpdate = now;

    Firebase.RTDB.setInt(&fbdo, "/pm1", pm1_final);
    Firebase.RTDB.setInt(&fbdo, "/pm25", pm25_final);
    Firebase.RTDB.setInt(&fbdo, "/pm10", pm10_final);
  }
}