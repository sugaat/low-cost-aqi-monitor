import serial
import pandas as pd
import time
import os

# ---------------- Serial setup ----------------
# Ensure 'COM5' matches your ESP32 port. On Linux/Mac use '/dev/ttyUSB0'
ser = serial.Serial('COM5', 115200, timeout=1)
time.sleep(2)  # Wait for ESP32 to reboot after connection

# ---------------- Data storage ----------------
pm1_list = []
pm25_list = []
pm10_list = []

# Time tracking
window_start_time = time.time()
output_file = "pm_readings_1min_avg.csv"

print(f"Logging started. Saving 1-minute averages to {output_file}...")
print("Press Ctrl+C to stop.")

# ---------------- Logging loop ----------------
try:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()

        if line and "PM1.0" in line:
            parts = line.split("|")
            try:
                # Changed int() to float() to handle calibrated decimals
                pm1 = float(parts[0].split(":")[1].strip())
                pm25 = float(parts[1].split(":")[1].strip())
                pm10 = float(parts[2].split(":")[1].strip())
                
                pm1_list.append(pm1)
                pm25_list.append(pm25)
                pm10_list.append(pm10)
                
                # Optional: print raw data to see progress
                print(f"Reading: PM1={pm1}, PM2.5={pm25}, PM10={pm10}", end='\r')
                
            except (IndexError, ValueError) as e:
                # Skip lines that are malformed or missing data
                continue

        # -------- Check if 1 minute (60 seconds) is over --------
        if time.time() - window_start_time >= 60:
            if pm1_list:  
                avg_pm1 = sum(pm1_list) / len(pm1_list)
                avg_pm25 = sum(pm25_list) / len(pm25_list)
                avg_pm10 = sum(pm10_list) / len(pm10_list)

                timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

                df = pd.DataFrame([{
                    "Timestamp": timestamp,
                    "PM1.0_avg": round(avg_pm1, 2),
                    "PM2.5_avg": round(avg_pm25, 2),
                    "PM10_avg": round(avg_pm10, 2),
                    "Samples": len(pm1_list) # Useful to see how many readings were averaged
                }])

                # Append to CSV
                df.to_csv(
                    output_file,
                    mode='a',
                    index=False,
                    header=not os.path.exists(output_file)
                )

                print(f"\nSaved at {timestamp} -> PM2.5 Avg: {avg_pm25:.2f} (from {len(pm1_list)} samples)")

            # -------- Reset for next minute --------
            pm1_list.clear()
            pm25_list.clear()
            pm10_list.clear()
            window_start_time = time.time()

except KeyboardInterrupt:
    print("\nStopping...")
finally:
    ser.close()
    print("Serial port closed. Data logging stopped safely.")