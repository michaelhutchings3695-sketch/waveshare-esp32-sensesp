#pragma once

/**
 * Configuration header for Waveshare ESP32-S3-POE-ETH-8DI-8RO
 * Contains all pin definitions and hardware configuration
 */

// ===== Device Identification =====
#define DEVICE_NAME "waveshare001"
#define DEVICE_FRIENDLY "WaveShare001"
#define DEVICE_COMMENT "WaveShare ESP32-S3-POE-ETH-8DI-8RO device"

// ===== I2C Configuration =====
#define I2C_SDA_PIN 42
#define I2C_SCL_PIN 41
#define I2C_FREQUENCY 100000  // 100 kHz
#define I2C_BUS Wire

// ===== PCA9554 GPIO Expander =====
#define PCA9554_ADDRESS 0x20
#define PCA9554_SDA I2C_SDA_PIN
#define PCA9554_SCL I2C_SCL_PIN

// ===== Ethernet (W5500) Configuration =====
#define ETH_TYPE W5500
#define ETH_CLK_PIN 15
#define ETH_MOSI_PIN 13
#define ETH_MISO_PIN 14
#define ETH_CS_PIN 16
#define ETH_INT_PIN 12
#define ETH_RST_PIN -1  // No reset pin
#define ETH_SPI_FREQ 40000000  // 40 MHz

// ===== UART Configuration (Modbus) =====
#define MODBUS_UART_NUM 1
#define MODBUS_TX_PIN 17
#define MODBUS_RX_PIN 18
#define MODBUS_BAUD_RATE 38400
#define MODBUS_DATA_BITS 8
#define MODBUS_STOP_BITS 1
#define MODBUS_PARITY 1  // 1 = EVEN

// ===== Digital Inputs (DI1-DI8) =====
#define DI_COUNT 8
#define DI1_PIN 4
#define DI2_PIN 5
#define DI3_PIN 6
#define DI4_PIN 7
#define DI5_PIN 8
#define DI6_PIN 9
#define DI7_PIN 10
#define DI8_PIN 11

// Array of DI pins for iteration
const uint8_t DI_PINS[DI_COUNT] = {DI1_PIN, DI2_PIN, DI3_PIN, DI4_PIN, 
                                   DI5_PIN, DI6_PIN, DI7_PIN, DI8_PIN};

const char* DI_NAMES[DI_COUNT] = {"DI1", "DI2", "DI3", "DI4", 
                                   "DI5", "DI6", "DI7", "DI8"};

// DI debounce settings
#define DI_DEBOUNCE_MS 10
#define DI_INPUT_MODE INPUT_PULLUP
#define DI_INVERTED true  // Low = active

// ===== Relay Outputs (via PCA9554) =====
#define RELAY_COUNT 8
#define RELAY1_PIN 0
#define RELAY2_PIN 1
#define RELAY3_PIN 2
#define RELAY4_PIN 3
#define RELAY5_PIN 4
#define RELAY6_PIN 5
#define RELAY7_PIN 6
#define RELAY8_PIN 7

// Array of relay pins for iteration
const uint8_t RELAY_PINS[RELAY_COUNT] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN,
                                         RELAY5_PIN, RELAY6_PIN, RELAY7_PIN, RELAY8_PIN};

const char* RELAY_NAMES[RELAY_COUNT] = {"Relay 1", "Relay 2", "Relay 3", "Relay 4",
                                         "Relay 5", "Relay 6", "Relay 7", "Relay 8"};

const char* RELAY_SK_PATHS[RELAY_COUNT] = {
  "electrical.switches.relay1",
  "electrical.switches.relay2",
  "electrical.switches.relay3",
  "electrical.switches.relay4",
  "electrical.switches.relay5",
  "electrical.switches.relay6",
  "electrical.switches.relay7",
  "electrical.switches.relay8"
};

// ===== Buzzer Configuration =====
#define BUZZER_PIN 46
#define BUZZER_CHANNEL 0  // LEDC channel
#define BUZZER_FREQUENCY 1000  // Hz
#define BUZZER_RESOLUTION 8  // bits (0-255)

// ===== RGB LED Configuration =====
#define RGB_LED_PIN 38
#define RGB_LED_COUNT 1
#define RGB_LED_TYPE NEO_RGB  // WS2812 color order
#define RGB_LED_BRIGHTNESS 100  // 0-255

// ===== RTC (PCF85063) =====
#define RTC_ADDRESS 0x51
#define RTC_SDA I2C_SDA_PIN
#define RTC_SCL I2C_SCL_PIN

// ===== Boot Button =====
#define BOOT_BUTTON_PIN 0
#define BOOT_BUTTON_PULLUP true
#define BOOT_BUTTON_INVERTED true

// ===== Signal K Configuration =====
#define SENSESP_HOSTNAME DEVICE_NAME
#define SENSESP_SSID_PREFIX DEVICE_NAME

// Signal K paths for digital inputs
const char* DI_SK_PATHS[DI_COUNT] = {
  "electrical.switches.di1",
  "electrical.switches.di2",
  "electrical.switches.di3",
  "electrical.switches.di4",
  "electrical.switches.di5",
  "electrical.switches.di6",
  "electrical.switches.di7",
  "electrical.switches.di8"
};

// Signal K paths for network info
#define SK_NETWORK_IP "electrical.network.eth.ip"
#define SK_NETWORK_MAC "electrical.network.eth.mac"
#define SK_NETWORK_GATEWAY "electrical.network.eth.gateway"
#define SK_NETWORK_DNS "electrical.network.eth.dns"

// Signal K paths for buzzer and LED
#define SK_BUZZER "electrical.alarms.buzzer"
#define SK_RGB_LED "navigation.lights.rgb"

// ===== System Configuration =====
#define ENABLE_WEB_UI true
#define ENABLE_OTA_UPDATES true
#define ENABLE_LOGGING true
#define LOG_LEVEL ESP_LOG_INFO

// WiFi configuration (optional backup, Ethernet is primary)
#define ENABLE_WIFI_AP_MODE true
#define WIFI_AP_CHANNEL 1
#define WIFI_AP_PASSWORD "changeme"

#endif  // CONFIG_H
