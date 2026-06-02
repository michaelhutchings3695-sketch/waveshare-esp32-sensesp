#pragma once

#include "config.h"
#include <Wire.h>

/**
 * I2C Bus Setup and Management
 * Handles initialization of I2C for PCA9554, PCF85063, and other devices
 */

class I2CManager {
public:
  static void setup() {
    debugI("Initializing I2C bus (SDA=%d, SCL=%d, %d Hz)...", 
           I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY);

    // Initialize I2C bus
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY);

    // Scan for connected devices
    scanBus();
  }

  static void scanBus() {
    debugI("I2C Bus Scan:");
    byte error, address;
    int nDevices = 0;

    for (address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();

      if (error == 0) {
        debugI("  Device found at address 0x%02X", address);
        nDevices++;
      } else if (error == 4) {
        debugE("  Unknown error at address 0x%02X", address);
      }
    }

    if (nDevices == 0) {
      debugW("No I2C devices found!");
    } else {
      debugI("Total devices found: %d", nDevices);
    }
  }

  // Helper to read from I2C device
  static bool readRegister(uint8_t address, uint8_t reg, uint8_t& value) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
      return false;
    }

    if (Wire.requestFrom(address, 1) != 1) {
      return false;
    }

    value = Wire.read();
    return true;
  }

  // Helper to write to I2C device
  static bool writeRegister(uint8_t address, uint8_t reg, uint8_t value) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
  }
};

#endif  // I2C_SETUP_H
