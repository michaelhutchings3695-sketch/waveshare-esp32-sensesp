#pragma once

#include "config.h"
#include <ETH.h>

/**
 * Ethernet Configuration for W5500
 * Handles Ethernet initialization and connection management
 */

class EthernetConfig {
public:
  static void setup() {
    debugI("Initializing Ethernet (W5500)...");

    // Configure SPI pins
    SPI.begin(ETH_CLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN, ETH_CS_PIN);

    // Initialize Ethernet
    ETH.begin(
      ETH_TYPE,
      ETH_MOSI_PIN,
      ETH_MISO_PIN,
      ETH_CLK_PIN,
      ETH_CS_PIN,
      ETH_INT_PIN
    );

    // Wait for connection
    int timeout = 0;
    while (!ETH.linkUp() && timeout < 100) {
      delay(100);
      timeout++;
    }

    if (ETH.linkUp()) {
      debugI("Ethernet Connected!");
      debugI("IP Address: %s", ETH.localIP().toString().c_str());
      debugI("MAC Address: %s", ETH.macAddress().c_str());
    } else {
      debugE("Ethernet connection failed!");
    }
  }

  static String getIPAddress() {
    return ETH.linkUp() ? ETH.localIP().toString() : "Not connected";
  }

  static String getMACAddress() {
    return ETH.macAddress();
  }

  static String getGateway() {
    return ETH.gatewayIP().toString();
  }

  static String getDNS() {
    return ETH.dnsIP().toString();
  }

  static bool isConnected() {
    return ETH.linkUp();
  }
};

#endif  // ETHERNET_CONFIG_H
