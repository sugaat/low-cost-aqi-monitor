#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PMS.h>

// ---------- LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- PMS ----------
HardwareSerial pmsSerial(2);
PMS pms(pmsSerial);
PMS::DATA data;

// ---------- Pins ----------
const int potPin = 34;

// ---------- Calibration ----------
const float PM1_A  = 0.411; const float PM1_B  = 3.54;
const float PM25_A = 0.342; const float PM25_B = 6.11;
const float PM10_A = 1.87;  const float PM10_B = -45.44;

// ---------- Data ----------
int pm1_final = 0;
int pm25_final = 0;
int pm10_final = 0;

// ---------- Timing ----------
unsigned long lastLcdUpdate = 0;
const int LCD_INTERVAL = 250;

void setup() {
  Serial.begin(115200);

  // PMS (TX=27, RX=26)
  pmsSerial.begin(9600, SERIAL_8N1, 27, 26);

  // LCD (SDA=33, SCL=32)
  Wire.begin(33, 32);
  lcd.begin(16, 2);
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("PM Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);
}

void loop() {

  // ---------- READ PMS ----------
  if (pms.read(data)) {

    float c1  = PM1_A  * data.PM_AE_UG_1_0 + PM1_B;
    float c25 = PM25_A * data.PM_AE_UG_2_5 + PM25_B;
    float c10 = PM10_A * data.PM_AE_UG_10_0 + PM10_B;

    pm1_final  = (c1 < 0) ? 0 : (int)c1;
    pm25_final = (c25 < 0) ? 0 : (int)c25;
    pm10_final = (c10 < 0) ? 0 : (int)c10;
  }

  // ---------- LCD UPDATE ----------
  unsigned long now = millis();

  if (now - lastLcdUpdate >= LCD_INTERVAL) {
    lastLcdUpdate = now;

    int potValue = analogRead(potPin);

    lcd.clear();

    // ---------- SCREEN 1 ----------
    if (potValue < 1365) {
      lcd.setCursor(0, 0);
      lcd.print("PM1:"); lcd.print(pm1_final);
      lcd.setCursor(0, 1);
      lcd.print("PM2.5:"); lcd.print(pm25_final);
    }

    // ---------- SCREEN 2 ----------
    else if (potValue < 2730) {
      lcd.setCursor(0, 0);
      lcd.print("PM10:");
      lcd.print(pm10_final);
      lcd.setCursor(0, 1);
      lcd.print("Air Quality");
    }

    // ---------- SCREEN 3 ----------
    else {
      lcd.setCursor(0, 0);
      lcd.print("PM Monitor");
      lcd.setCursor(0, 1);
      lcd.print("..........");
    }
  }
}