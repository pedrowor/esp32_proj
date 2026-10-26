# ESP32 microSD to HTTP Upload Prototype

An experimental **ESP32 / Arduino C++** project that scans Wi-Fi networks, connects using configured credentials, reads files on a microSD card using SPI, and **attempts** to upload those files to a configured HTTP endpoint.

**Status:** Educational prototype. Reliable remote file upload has **not** been verified from the repository source, and the current HTTP request/stream sequence needs testing and likely correction before real-world use.

## Hardware and wiring
The sketch uses ESP32 GPIO pins for the microSD SPI bus:

| microSD module | ESP32 |
| --- | --- |
| CS | GPIO 5 |
| SCK | GPIO 18 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| VCC | 3.3 V (only with a compatible module) |
| GND | GND |

Check the voltage requirements of your particular SD module before connecting it.

## Source layout
```text
proj_esp32/SKETCH_FINAL/SKETCH_FINAL.ino
```

## Intended workflow
1. Start serial output at 115200 baud and scan available Wi-Fi networks.
2. Attempt connection to the configured SSID.
3. Initialize the SD card using SPI.
4. Iterate over files in the root directory.
5. Attempt to send file data to a configured Apps Script / HTTP destination.

## Setup and safety
1. Install Arduino IDE and the ESP32 board support package.
2. Open `SKETCH_FINAL.ino`, select your ESP32 board and serial port.
3. Configure the Wi-Fi SSID and password **locally**; never commit real credentials.
4. Configure an upload endpoint that you control.
5. Connect a compatible SD reader, upload the sketch, and review the Serial Monitor at 115200 baud.

**Security note:** An Apps Script endpoint is currently hard-coded in the committed sketch. Treat it as potentially exposed; review access permissions and replace or disable it if necessary. Do not reuse the public endpoint for sensitive data.

## Known limitations
- The upload routine writes to the HTTP client's stream before issuing a POST with an empty body. This implementation should not be assumed to upload files successfully.
- There is no robust retry queue, upload-integrity verification, successful-upload deletion policy, or persistent upload state.
- Wi-Fi configuration is not interactive despite the network scan.
- The main `loop()` is empty; uploads are attempted once during setup.
- Testing requires physical hardware and an endpoint you control.

## Skills practiced
ESP32 firmware, SPI, SD storage, Wi-Fi diagnostics, serial logging, HTTP integration, and hardware/software troubleshooting.
