# Waveshare ESP32-S3-POE-ETH-8DI-8RO SensESP Firmware

This repository contains a complete SensESP firmware implementation for the Waveshare ESP32-S3-POE-ETH-8DI-8RO device.

## Device Overview

- **Microcontroller**: ESP32-S3-WROOM-1U-N16R8
- **Flash Memory**: 16 MB
- **PSRAM**: 8 MB (Octal mode, 80MHz)
- **Network**: Ethernet (W5500)
- **Connectivity**: Signal K over MQTT
- **Digital Inputs**: 8 (DI1-DI8)
- **Digital Outputs/Relays**: 8 (Relay1-Relay8) via PCA9554 GPIO expander
- **I2C**: RTC (PCF85063), GPIO expander (PCA9554)
- **UART**: Modbus interface
- **Audio**: Buzzer with LEDC PWM
- **Status Indicator**: RGB LED (WS2812)

## Features Converted from ESPHome

✅ Ethernet connectivity (W5500)
✅ 8 Digital inputs with debouncing
✅ 8 Relay outputs via PCA9554 GPIO expander
✅ I2C communication (SDA: GPIO42, SCL: GPIO41)
✅ Modbus UART interface (38400 baud, 8E1)
✅ Real-time clock (PCF85063)
✅ Buzzer output (GPIO46, LEDC PWM)
✅ RGB LED status indicator (GPIO38, WS2812)
✅ Signal K integration
✅ OTA firmware updates
✅ Web configuration interface
✅ Restart/Factory reset buttons

## Project Structure

```
.
├── platformio.ini          # PlatformIO build configuration
├── include/
│   ├── config.h           # Hardware pin definitions and configuration
│   ├── gpio_inputs.h      # Digital input handlers
│   ├── relay_outputs.h    # Relay output handlers
│   ├── ethernet_config.h  # Ethernet setup
│   ├── i2c_setup.h        # I2C initialization
│   └── led_buzzer.h       # RGB LED and Buzzer control
├── src/
│   └── main.cpp           # Main application code
├── lib/                   # Custom libraries (if any)
├── data/                  # Web UI assets (auto-generated)
└── README.md
```

## Installation

### Prerequisites

- **PlatformIO CLI** or **Visual Studio Code with PlatformIO extension**
- **USB-to-Serial adapter** (CH340 or similar) for initial programming
- **Python 3.8+**

### Setup Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/michaelhutchings3695-sketch/waveshare-esp32-sensesp.git
   cd waveshare-esp32-sensesp
   ```

2. **Install dependencies**
   ```bash
   pio pkg install
   ```

3. **Build the firmware**
   ```bash
   pio run -e waveshare_esp32s3poe
   ```

4. **Upload to device**
   ```bash
   pio run -e waveshare_esp32s3poe -t upload
   ```

5. **Monitor serial output**
   ```bash
   pio device monitor -b 115200
   ```

## Configuration

### Pin Configuration

All pin mappings are defined in `include/config.h`:

- **I2C**: SDA=GPIO42, SCL=GPIO41 (100kHz)
- **Ethernet**: CLK=GPIO15, MOSI=GPIO13, MISO=GPIO14, CS=GPIO16, INT=GPIO12
- **UART (Modbus)**: TX=GPIO17, RX=GPIO18 (38400 baud, 8E1)
- **Digital Inputs (DI1-DI8)**: GPIO4-GPIO11
- **Buzzer**: GPIO46 (LEDC)
- **RGB LED**: GPIO38 (WS2812)
- **PCA9554 Relays**: Address 0x20, pins 0-7

### Signal K Paths

The firmware maps hardware to Signal K paths automatically:

```
electrical.switches.di1          # Digital Input 1 (binary)
electrical.switches.di2          # Digital Input 2 (binary)
...
electrical.switches.di8          # Digital Input 8 (binary)

electrical.switches.relay1       # Relay 1 (switchable)
electrical.switches.relay2       # Relay 2 (switchable)
...
electrical.switches.relay8       # Relay 8 (switchable)

electrical.alarms.buzzer         # Buzzer control
navigation.lights.rgb            # RGB LED status

navigation.datetime              # Time from RTC
electrical.network.eth.ip        # Ethernet IP address
electrical.network.eth.mac       # MAC address
```

## Network Configuration

### Initial Setup

1. Power the device via PoE or external 12-24V power supply
2. The device will broadcast a WiFi AP with SSID `waveshare001_AP` during initial setup
3. Connect to this AP and open `http://192.168.4.1` in a browser
4. Configure WiFi or Ethernet settings (Ethernet is hardwired, WiFi is optional)

### Signal K Connection

1. Access the web UI at `http://<device-ip>`
2. Navigate to Configuration → Signal K
3. Enter your Signal K server details (hostname/IP and port)
4. Configure MQTT broker if using MQTT transport

## Usage Examples

### Reading Digital Inputs

Digital inputs DI1-DI8 are automatically published to Signal K when they change:

```json
{
  "path": "electrical.switches.di1",
  "value": true,
  "timestamp": "2025-06-02T10:30:00.000Z"
}
```

### Controlling Relays

Relays can be controlled via Signal K or the web UI:

```json
{
  "context": "vessels.self",
  "put": [
    {
      "path": "electrical.switches.relay1",
      "value": true
    }
  ]
}
```

### Modbus Integration

Modbus sensors can be added via the web configuration. See examples in `include/modbus_handlers.h`.

## Troubleshooting

### Device won't connect to network

1. Verify Ethernet cable is connected
2. Check W5500 SPI pins (CLK, MOSI, MISO, CS, INT)
3. Inspect PCF85063 I2C connection for clock drift

### Digital inputs not responding

1. Check GPIO pins are not in strapping conflict
2. Verify INPUT_PULLUP is configured correctly
3. Ensure debounce filter is appropriate (10ms default)

### Relays not switching

1. Verify PCA9554 I2C address is 0x20
2. Check relay coil power supply
3. Test relay control via web UI first

### Modbus communication issues

1. Verify UART pins (TX=GPIO17, RX=GPIO18)
2. Check baud rate (38400, 8E1)
3. Test with a Modbus protocol analyzer

## Development

### Building Custom Transforms

Add custom sensor transforms in `src/main.cpp`:

```cpp
auto transform = new LinearTransform(
  1.0,      // multiplier
  0.0,      // offset
  "/sensors/engine_temp",  // input path
  "engine.temperature",     // output Signal K path
  "Engine Temperature",      // display name
  "°C"                       // unit
);
```

### Adding Custom Outputs

Create custom outputs for device control:

```cpp
new ControlOutputInt(
  "electrical.switches.relay1",
  new RelayOutputHandler(RELAY1_PIN)
);
```

## References

- [SensESP Documentation](https://signalk.org/SensESP/)
- [SensESP GitHub Repository](https://github.com/SignalK/SensESP)
- [Signal K Standard](https://signalk.org/)
- [Waveshare ESP32-S3-POE-ETH Product Page](https://www.waveshare.com/wiki/ESP32-S3-POE-ETH)
- [PlatformIO Documentation](https://docs.platformio.org/)

## Community Support

- **Signal K Discord**: [Join here](https://discord.com/invite/j9V7hXrE4c) (#sensesp channel)
- **SensESP Discussions**: [GitHub Discussions](https://github.com/SignalK/SensESP/discussions)
- **Waveshare Support**: [Wiki & Community](https://www.waveshare.com/)

## License

MIT License - Feel free to modify and distribute

## Author

Converted from ESPHome configuration by michaelhutchings3695-sketch
