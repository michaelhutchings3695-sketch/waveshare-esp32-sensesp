/**
 * SensESP Firmware for Waveshare ESP32-S3-POE-ETH-8DI-8RO
 * 
 * Features:
 * - Ethernet connectivity (W5500)
 * - 8 Digital inputs with debouncing
 * - 8 Relay outputs via PCA9554 GPIO expander
 * - I2C bus for peripherals
 * - Modbus UART interface
 * - Real-time clock (PCF85063)
 * - Buzzer and RGB LED control
 * - Signal K integration
 * - Web configuration UI
 * - OTA firmware updates
 */

#include "Arduino.h"
#include "sensesp.h"
#include "sensesp_app.h"
#include "sensesp_app_builder.h"
#include "config.h"
#include "gpio_inputs.h"
#include "relay_outputs.h"
#include "ethernet_config.h"
#include "i2c_setup.h"
#include "led_buzzer.h"
#include <sensesp/sensesp_app.h>
#include <sensesp/signalk/signalk_value_listener.h>
#include <sensesp/sensors/digital_output.h>

// Define the GPIO pin connected to your relay
const int RELAY_PIN = 22; 

// Define the Signal K path your dashboard uses to control the relay
// Example: electrical.switches.anchorLight
const char* sk_path = "electrical.switches.anchorLight";

void setup() {
  // Setup the pin as an output and set it to LOW initially
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Initialize SensESP
  sensesp_app = new SensESPApp();

  // Create a listener that listens to the Signal K path
  auto* sk_listener = new SKValueListener<bool>(sk_path);

  // Connect the listener to a LambdaConsumer that triggers the relay
  sk_listener->connectTo(new LambdaConsumer<bool>([](bool payload) {
    if (payload) {
      digitalWrite(RELAY_PIN, HIGH); // Turn Relay ON
    } else {
      digitalWrite(RELAY_PIN, LOW);  // Turn Relay OFF
    }
  }));

  sensesp_app->start();
}

// Global instances
SensESPApp* sensesp_app = nullptr;
DigitalInputManager* di_manager = nullptr;
PCA9554RelayManager* relay_manager = nullptr;
RGBLEDController* rgb_led = nullptr;
BuzzerController* buzzer = nullptr;

/**
 * Setup function - called once at startup
 */
void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(1000);
  
  debugI("\n\n=== Waveshare ESP32-S3-POE-ETH-8DI-8RO SensESP Firmware ===");
  debugI("Device: %s (%s)", DEVICE_FRIENDLY, DEVICE_NAME);
  debugI("Comment: %s", DEVICE_COMMENT);
  debugI("Starting setup...");

  // Initialize RGB LED first (visual feedback)
  rgb_led = new RGBLEDController();
  rgb_led->setup();
  rgb_led->setStatusBusy();  // Blue = initializing

  // Initialize buzzer
  buzzer = new BuzzerController();
  buzzer->setup();
  buzzer->beep(100);  // Single beep on startup

  // Initialize I2C bus (for PCA9554 and RTC)
  I2CManager::setup();
  delay(100);

  // Initialize Ethernet
  EthernetConfig::setup();
  delay(500);

  // Initialize PSRAM
  if (psramFound()) {
    debugI("PSRAM: %d bytes total, %d bytes free", 
            ESP.getPsramSize(), ESP.getFreePsram());
  } else {
    debugW("PSRAM not found!");
  }

  // Create SensESP app
  debugI("Creating SensESP app...");
  sensesp_app = new SensESPApp();

  // Configure SensESP basic settings
  sensesp_app->set_hostname(DEVICE_NAME);
  sensesp_app->set_app_id(DEVICE_NAME);
  
  // Setup network info sensors (optional - for debugging)
  setupNetworkInfoSensors();

  // Initialize digital inputs
  debugI("Setting up digital inputs (DI1-DI8)...");
  di_manager = new DigitalInputManager();
  di_manager->setup();

  // Initialize relay outputs
  debugI("Setting up relay outputs (Relay1-Relay8) via PCA9554...");
  relay_manager = new PCA9554RelayManager(&Wire, PCA9554_ADDRESS);
  relay_manager->setup();

  // Setup Modbus UART (if needed for external sensors)
  setupModbusUART();

  // Setup RTC time synchronization
  setupRTCSync();

  // Start SensESP
  debugI("Starting SensESP app...");
  sensesp_app->start();

  // Visual feedback: Success
  rgb_led->setStatusOK();  // Green = ready
  buzzer->playTone(1000, 100);
  buzzer->playTone(1500, 100);

  debugI("Setup complete!");
  debugI("========================================================\n");
}

/**
 * Main loop - called repeatedly
 */
void loop() {
  // SensESP handles everything in the background
  // Just let the app tick
  sensesp_app->tick();

  // Optional: Add any additional monitoring here
  // For example, blink RGB LED or check system health
  delay(1);  // Small delay to allow other tasks to run
}

/**
 * Setup network information sensors
 * Publishes IP, MAC, Gateway, DNS to Signal K for monitoring
 */
void setupNetworkInfoSensors() {
  debugI("Setting up network info sensors...");

  // IP Address
  auto ip_producer = new StringProducer(
    SK_NETWORK_IP,
    new SKMetadata("IP Address", "Network Address")
  );
  ip_producer->set_input(EthernetConfig::getIPAddress());

  // MAC Address
  auto mac_producer = new StringProducer(
    SK_NETWORK_MAC,
    new SKMetadata("MAC Address", "Hardware Address")
  );
  mac_producer->set_input(EthernetConfig::getMACAddress());

  // Gateway
  auto gateway_producer = new StringProducer(
    SK_NETWORK_GATEWAY,
    new SKMetadata("Gateway", "Network Gateway")
  );
  gateway_producer->set_input(EthernetConfig::getGateway());

  // DNS
  auto dns_producer = new StringProducer(
    SK_NETWORK_DNS,
    new SKMetadata("DNS", "DNS Server")
  );
  dns_producer->set_input(EthernetConfig::getDNS());
}

/**
 * Setup Modbus UART interface
 * For connecting to Modbus devices on the RS485/UART interface
 */
void setupModbusUART() {
  debugI("Setting up Modbus UART (TX=%d, RX=%d, %d baud)...",
          MODBUS_TX_PIN, MODBUS_RX_PIN, MODBUS_BAUD_RATE);

  // Initialize UART1 for Modbus
  Serial1.begin(
    MODBUS_BAUD_RATE,
    SERIAL_8E1,  // 8 bits, Even parity, 1 stop bit
    MODBUS_RX_PIN,
    MODBUS_TX_PIN
  );

  debugI("Modbus UART initialized.");

  // Note: To add Modbus sensor support, you would typically:
  // 1. Include a Modbus library (e.g., libmodbus, Arduino-Modbus)
  // 2. Create sensor transforms that read Modbus registers
  // 3. Map Modbus values to Signal K paths
  // 
  // Example sensor setup would go here
}

/**
 * Setup RTC time synchronization
 * Synchronizes system time with PCF85063 RTC chip
 */
void setupRTCSync() {
  debugI("Setting up RTC (PCF85063) at address 0x%02X...", RTC_ADDRESS);

  // Read time from RTC
  uint8_t seconds = 0;
  if (I2CManager::readRegister(RTC_ADDRESS, 0x04, seconds)) {
    debugI("RTC is present and responding.");
  } else {
    debugW("RTC not responding at address 0x%02X", RTC_ADDRESS);
  }

  // Note: Full RTC driver implementation would include:
  // 1. Reading current time from RTC
  // 2. Setting system time if valid
  // 3. Periodic synchronization
  // 4. Battery-backed timekeeping
}

/**
 * Utility: Print system information to console
 */
void printSystemInfo() {
  debugI("\n=== System Information ===");
  debugI("Device: %s", DEVICE_FRIENDLY);
  debugI("Hostname: %s", DEVICE_NAME);
  debugI("\nMemory:");
  debugI("  Flash: %d bytes", ESP.getFlashChipSize());
  debugI("  RAM: %d bytes", ESP.getTotalHeap());
  debugI("  Free RAM: %d bytes", ESP.getFreeHeap());
  debugI("  PSRAM: %d bytes", ESP.getPsramSize());
  debugI("  Free PSRAM: %d bytes", ESP.getFreePsram());
  debugI("\nNetwork:");
  debugI("  IP: %s", EthernetConfig::getIPAddress().c_str());
  debugI("  MAC: %s", EthernetConfig::getMACAddress().c_str());
  debugI("  Gateway: %s", EthernetConfig::getGateway().c_str());
  debugI("  DNS: %s", EthernetConfig::getDNS().c_str());
  debugI("  Connected: %s", EthernetConfig::isConnected() ? "Yes" : "No");
  debugI("\nCPU:");
  debugI("  Frequency: %d MHz", getCpuFrequencyMhz());
  debugI("  Cores: 2");
  debugI("==========================\n");
}
