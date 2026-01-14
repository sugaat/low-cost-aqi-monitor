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

void setup() {
  Serial.begin(115200);

  // PMS7003 UART on safe pins
  // TX (PMS) -> GPIO18 (ESP RX)
  // RX (PMS) -> GPIO19 (ESP TX)
  pmsSerial.begin(9600, SERIAL_8N1, 18, 19);

  // LCD I2C
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("PMS7003 Init");
  lcd.setCursor(0, 1);
  lcd.print("Please wait");

  Serial.println("Monitoring PMS7003...");
}

void loop() {
  if (pms.read(data)) {
    if (millis() - lastUpdate >= interval) {
      // ---- Serial Output ----
      Serial.print("PM1.0: ");
      Serial.print(data.PM_AE_UG_1_0);
      Serial.print(" | PM2.5: ");
      Serial.print(data.PM_AE_UG_2_5);
      Serial.print(" | PM10: ");
      Serial.println(data.PM_AE_UG_10_0);

      // ---- LCD Output ----
      lcd.clear();

      // Row 0: PM1.0 and PM2.5
      lcd.setCursor(0, 0);
      lcd.print("P1:");
      lcd.print(data.PM_AE_UG_1_0);
      lcd.print(" P2:");
      lcd.print(data.PM_AE_UG_2_5);

      // Row 1: PM10
      lcd.setCursor(0, 1);
      lcd.print("P10:");
      lcd.print(data.PM_AE_UG_10_0);

      lastUpdate = millis();
    }
  }
}
