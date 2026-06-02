#pragma once

#include "sensesp.h"
#include "config.h"
#include <Wire.h>

/**
 * Relay Output Handler
 * Manages the 8 relay outputs (Relay1-Relay8) via PCA9554 GPIO expander
 */

class PCA9554RelayManager {
private:
  TwoWire* i2c_bus;
  uint8_t address;
  uint8_t output_state = 0x00;  // All relays off initially

public:
  PCA9554RelayManager(TwoWire* bus = &Wire, uint8_t addr = PCA9554_ADDRESS)
    : i2c_bus(bus), address(addr) {}

  void setup() {
    // Configure PCA9554 pins 0-7 as outputs
    // PCA9554 register 0x03 = output configuration
    i2c_bus->beginTransmission(address);
    i2c_bus->write(0x03);      // Configuration register
    i2c_bus->write(0x00);      // All pins as outputs
    i2c_bus->endTransmission();

    delay(10);

    // Initialize all relays to OFF
    setAllRelays(0x00);

    // Create relay output controls and connect to Signal K
    for (int i = 0; i < RELAY_COUNT; i++) {
      setupRelay(i);
    }
  }

  void setRelay(uint8_t relay_index, bool state) {
    if (relay_index >= RELAY_COUNT) return;

    uint8_t bit = (1 << relay_index);
    if (state) {
      output_state |= bit;
    } else {
      output_state &= ~bit;
    }

    writeState();
    debugD("Relay %d set to %d", relay_index + 1, state);
  }

  bool getRelay(uint8_t relay_index) const {
    if (relay_index >= RELAY_COUNT) return false;
    return (output_state & (1 << relay_index)) != 0;
  }

private:
  void setupRelay(int index) {
    const char* name = RELAY_NAMES[index];
    const char* sk_path = RELAY_SK_PATHS[index];

    // Create controllable output
    auto relay_output = new PCA9554RelayOutput(this, index, name, sk_path);
    sensesp_app->add_config_item(relay_output);
  }

  void setAllRelays(uint8_t state) {
    output_state = state;
    writeState();
  }

  void writeState() {
    i2c_bus->beginTransmission(address);
    i2c_bus->write(0x01);         // Output port register
    i2c_bus->write(output_state);
    i2c_bus->endTransmission();
  }

  friend class PCA9554RelayOutput;
};

/**
 * PCA9554 Relay Output
 * Individual controllable relay output
 */
class PCA9554RelayOutput : public ControlOutput<bool> {
private:
  PCA9554RelayManager* manager;
  uint8_t relay_index;

public:
  PCA9554RelayOutput(PCA9554RelayManager* mgr, uint8_t index, const char* name, const char* sk_path)
    : ControlOutput<bool>(sk_path, name, new SKMetadata(name, "Switch", "Binary")),
      manager(mgr),
      relay_index(index) {}

  void set_input(const bool& input, uint64_t timestamp = 0) override {
    manager->setRelay(relay_index, input);
    ControlOutput<bool>::set_input(input, timestamp);
  }
};

#endif  // RELAY_OUTPUTS_H
