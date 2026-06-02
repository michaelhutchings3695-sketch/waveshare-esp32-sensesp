#pragma once

#include "config.h"
#include <Adafruit_NeoPixel.h>

/**
 * LED and Buzzer Control
 * Manages RGB LED (WS2812) and buzzer (PWM) outputs
 */

class RGBLEDController {
private:
  Adafruit_NeoPixel pixels;
  uint32_t current_color = 0;

public:
  RGBLEDController()
    : pixels(RGB_LED_COUNT, RGB_LED_PIN, NEO_GRB + NEO_KHZ800) {}

  void setup() {
    debugI("Initializing RGB LED on GPIO %d", RGB_LED_PIN);
    pixels.begin();
    setColor(0, 128, 0);  // Green - startup
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b) {
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.show();
    current_color = pixels.Color(r, g, b);
    debugD("RGB LED set to (%d, %d, %d)", r, g, b);
  }

  void setColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
    pixels.clear();
    uint32_t color = pixels.ColorHSV(hue, sat, val);
    pixels.setPixelColor(0, color);
    pixels.show();
    current_color = color;
  }

  // Status indicators
  void setStatusOK() {
    setColor(0, 255, 0);  // Green
  }

  void setStatusWarning() {
    setColor(255, 165, 0);  // Orange
  }

  void setStatusError() {
    setColor(255, 0, 0);  // Red
  }

  void setStatusBusy() {
    setColor(0, 0, 255);  // Blue
  }

  void pulse(uint8_t r, uint8_t g, uint8_t b, uint32_t duration_ms) {
    for (int i = 0; i < 256; i += 10) {
      setColor(r * i / 255, g * i / 255, b * i / 255);
      delay(duration_ms / 25);
    }
    for (int i = 255; i >= 0; i -= 10) {
      setColor(r * i / 255, g * i / 255, b * i / 255);
      delay(duration_ms / 25);
    }
  }
};

/**
 * Buzzer Controller
 * Manages LEDC PWM output for buzzer
 */
class BuzzerController {
private:
  static const int LEDC_TIMER_BITS = 8;  // 8-bit resolution
  static const int LEDC_CHANNEL = 0;

public:
  void setup() {
    debugI("Initializing Buzzer on GPIO %d", BUZZER_PIN);
    
    // Configure LEDC PWM
    ledcSetup(LEDC_CHANNEL, BUZZER_FREQUENCY, LEDC_TIMER_BITS);
    ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);
    
    // Start silent
    silence();
  }

  void beep(uint32_t duration_ms, uint8_t frequency = 1000) {
    setFrequency(frequency);
    setVolume(200);  // 80% volume
    delay(duration_ms);
    silence();
  }

  void playTone(uint32_t frequency, uint32_t duration_ms, uint8_t volume = 200) {
    setFrequency(frequency);
    setVolume(volume);
    delay(duration_ms);
    silence();
  }

  void setFrequency(uint32_t frequency) {
    // Note: LEDC frequency change requires reconfiguration
    ledcWriteFreq(LEDC_CHANNEL, frequency);
  }

  void setVolume(uint8_t volume) {
    // PWM duty cycle controls volume (0-255)
    ledcWrite(LEDC_CHANNEL, volume);
  }

  void silence() {
    ledcWrite(LEDC_CHANNEL, 0);
  }

  // Simple melody: array of {frequency, duration_ms} pairs
  void playMelody(const int melody[][2], int length) {
    for (int i = 0; i < length; i++) {
      if (melody[i][0] > 0) {
        playTone(melody[i][0], melody[i][1], 180);
      } else {
        delay(melody[i][1]);  // Rest
      }
      delay(50);  // Gap between notes
    }
  }
};

#endif  // LED_BUZZER_H
