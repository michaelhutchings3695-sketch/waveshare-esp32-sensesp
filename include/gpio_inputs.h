#pragma once

#include "sensesp.h"
#include "config.h"

/**
 * Digital Input Handler
 * Manages the 8 digital inputs (DI1-DI8) with debouncing and Signal K integration
 */

class DigitalInputManager {
public:
  DigitalInputManager() {}

  void setup() {
    for (int i = 0; i < DI_COUNT; i++) {
      setupDigitalInput(i);
    }
  }

private:
  void setupDigitalInput(int index) {
    uint8_t pin = DI_PINS[index];
    const char* name = DI_NAMES[index];
    const char* sk_path = DI_SK_PATHS[index];

    // Configure GPIO
    pinMode(pin, DI_INPUT_MODE);

    // Create binary sensor and connect to Signal K
    auto di_sensor = new BoolProducer(
      sk_path,
      new SKMetadata(
        name,
        "Binary Switch",
        "Discrete",
        1  // Number of connections
      )
    );

    // Create GPIO input source with debouncing
    auto di_input = new DigitalInputState(
      pin,
      DI_DEBOUNCE_MS,
      DI_INVERTED,  // inverted = low is active
      di_sensor
    );

    // Register with SensESP app
    sensesp_app->add_config_item(di_input);
  }
};

/**
 * Simple GPIO Digital Input State Handler
 * Reads GPIO pin state with debouncing
 */
class DigitalInputState : public Sensor {
private:
  uint8_t pin;
  uint32_t debounce_ms;
  bool inverted;
  unsigned long last_change_time = 0;
  bool last_state = false;
  BoolProducer* producer;

public:
  DigitalInputState(uint8_t _pin, uint32_t _debounce_ms, bool _inverted, BoolProducer* _producer)
    : pin(_pin), debounce_ms(_debounce_ms), inverted(_inverted), producer(_producer) {
    pinMode(pin, INPUT_PULLUP);
  }

  void tick() override {
    bool current_state = digitalRead(pin);
    if (inverted) {
      current_state = !current_state;
    }

    unsigned long now = millis();
    if (current_state != last_state) {
      if (now - last_change_time >= debounce_ms) {
        last_state = current_state;
        producer->set_input(current_state);
        debugD("DI%d changed to %d", pin, current_state);
      }
    } else {
      last_change_time = now;
    }
  }
};

#endif  // GPIO_INPUTS_H
