#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PMS.h>

// ---------- LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- PMS7003 ----------
HardwareSerial pmsSerial(2); // UART2
PMS pms(pmsSerial);
PMS::DATA data;

// ---------- Timing ----------
unsigned long lastUpdate = 0;
const unsigned long interval = 2000; // 2 seconds

// ---------- Calibration coefficients ----------
const float PM1_A  = 0.411;
const float PM1_B  = 3.54;

const float PM25_A = 0.342;
const float PM25_B = 6.11;

const float PM10_A = 1.87;
const float PM10_B = -45.44;

void setup() {
  Serial.begin(115200);

  // PMS7003 UART
  pmsSerial.begin(9600, SERIAL_8N1, 18, 19);

  // LCD I2C
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("PMS7003 Init");
  lcd.setCursor(0, 1);
  lcd.print("Calibrated");

  Serial.println("PMS7003 monitoring with calibration...");
}

void loop() {
  if (pms.read(data)) {
    if (millis() - lastUpdate >= interval) {

      // ---- Raw values ----
      float pm1_raw  = data.PM_AE_UG_1_0;
      float pm25_raw = data.PM_AE_UG_2_5;
      float pm10_raw = data.PM_AE_UG_10_0;

      // ---- Calibrated values ----
      float pm1_cal  = PM1_A  * pm1_raw  + PM1_B;
      float pm25_cal = PM25_A * pm25_raw + PM25_B;
      float pm10_cal = PM10_A * pm10_raw + PM10_B;

      // ---- Prevent negative values ----
      if (pm1_cal  < 0) pm1_cal  = 0;
      if (pm25_cal < 0) pm25_cal = 0;
      if (pm10_cal < 0) pm10_cal = 0;

      // ---- Serial Output ----
      Serial.print("PM1.0: ");
      Serial.print(pm1_cal, 1);
      Serial.print(" | PM2.5: ");
      Serial.print(pm25_cal, 1);
      Serial.print(" | PM10: ");
      Serial.println(pm10_cal, 1);

      // ---- LCD Output ----
      lcd.clear();

      // Row 0: PM1 & PM2.5
      lcd.setCursor(0, 0);
      lcd.print("P1:");
      lcd.print(pm1_cal, 0);
      lcd.print(" P2:");
      lcd.print(pm25_cal, 0);

      // Row 1: PM10
      lcd.setCursor(0, 1);
      lcd.print("P10:");
      lcd.print(pm10_cal, 0);

      lastUpdate = millis();
    }
  }
}
