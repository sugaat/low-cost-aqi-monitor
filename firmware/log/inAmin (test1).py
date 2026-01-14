import serial
import pandas as pd
import time
import os

# ---------------- Serial setup ----------------
ser = serial.Serial('COM5', 115200, timeout=1)
time.sleep(2)  # Wait for ESP32 reset

# ---------------- Data storage ----------------
pm1_list = []
pm25_list = []
pm10_list = []

# Time tracking
window_start_time = time.time()

# Output CSV file
output_file = "pm_readings_1min_avg.csv"

# ---------------- Logging loop ----------------
try:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()

        if line.startswith("PM1.0"):
            parts = line.split("|")
            try:
                pm1 = int(parts[0].split(":")[1].strip())
                pm25 = int(parts[1].split(":")[1].strip())
                pm10 = int(parts[2].split(":")[1].strip())
            except:
                continue  # skip bad line

            pm1_list.append(pm1)
            pm25_list.append(pm25)
            pm10_list.append(pm10)

        # -------- Check if 1 minute is over --------
        if time.time() - window_start_time >= 60:
            if pm1_list:  # avoid division by zero
                avg_pm1 = sum(pm1_list) / len(pm1_list)
                avg_pm25 = sum(pm25_list) / len(pm25_list)
                avg_pm10 = sum(pm10_list) / len(pm10_list)

                timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

                df = pd.DataFrame([{
                    "Timestamp": timestamp,
                    "PM1.0_avg": round(avg_pm1, 1),
                    "PM2.5_avg": round(avg_pm25, 1),
                    "PM10_avg": round(avg_pm10, 1)
                }])

                df.to_csv(
                    output_file,
                    mode='a',
                    index=False,
                    header=not os.path.exists(output_file)
                )

                print(f"1-min Avg → PM1:{avg_pm1:.1f}, PM2.5:{avg_pm25:.1f}, PM10:{avg_pm10:.1f}")

            # -------- Reset for next minute --------
            pm1_list.clear()
            pm25_list.clear()
            pm10_list.clear()
            window_start_time = time.time()

except KeyboardInterrupt:
    ser.close()
    print("Data logging stopped safely.")
