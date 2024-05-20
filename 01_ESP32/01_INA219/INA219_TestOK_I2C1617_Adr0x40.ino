#include <Wire.h>
#include <Adafruit_INA219.h>

// Create an instance of the Adafruit_INA219 class with the specified address
Adafruit_INA219 ina219(0x40);

void setup(void) {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  while (!Serial) {
      // Wait for serial port to connect. Needed for native USB
      delay(10);
  }

  // Initialize the I2C bus with custom SDA and SCL pins
  Wire.begin(16, 17);

  // Initialize the INA219 with the specified address
  if (!ina219.begin()) {
      Serial.println("Failed to find INA219 chip at address 0x40");
      while (1) { delay(10); }
  }

  // Optionally calibrate to a specific range (default range is 32V, 2A)
  // ina219.setCalibration_32V_2A();
  // ina219.setCalibration_32V_1A();
  // ina219.setCalibration_16V_400mA();
}

void loop(void) {
  float shuntVoltage_mV = 0;
  float busVoltage_V = 0;
  float current_mA = 0;
  float power_mW = 0;
  float loadVoltage_V = 0;

  // Read the values from the INA219
  shuntVoltage_mV = ina219.getShuntVoltage_mV();
  busVoltage_V = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);

  // Print the values to the Serial Monitor
  Serial.print("Bus Voltage:   "); Serial.print(busVoltage_V); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntVoltage_mV); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadVoltage_V); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  // Wait for a bit before reading again
  delay(2000);
}
