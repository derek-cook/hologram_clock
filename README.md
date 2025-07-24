# ESP32 TFT LCD Cube Clock

An ESP32-based digital clock display using TFT_eSPI library for LCD displays.

## Features

- WiFi-connected NTP time synchronization
- TFT LCD display with customizable clock interface
- Environment variable configuration for WiFi credentials
- Support for ESP32-S3 and other ESP32 variants

## Hardware Requirements

- ESP32-S3 (or other ESP32 variant)
- TFT LCD display compatible with TFT_eSPI library
- WiFi connection for time synchronization

## Setup Instructions

### 1. Install Dependencies

This project uses PlatformIO. Make sure you have PlatformIO installed.

### 2. Configure WiFi Credentials

Create a `.env` file in the project root directory with your WiFi credentials:

```bash
# Create .env file
touch .env
```

Add the following content to your `.env` file:

```
WIFI_SSID=YourWiFiNetworkName
WIFI_PASSWORD=YourWiFiPassword
```

**Important:** 
- Do not use quotes around the values
- Replace with your actual WiFi network name and password
- The `.env` file is automatically ignored by git for security

### 3. Configure TFT Display

Configure your TFT display settings in the TFT_eSPI library:

1. Locate the TFT_eSPI library folder in your PlatformIO environment
2. Edit `User_Setup.h` or create a custom setup file
3. Configure pins and display type according to your hardware

### 4. Build and Upload

```bash
# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

## Project Structure

```
QT Py LCD Cube/
├── src/
│   ├── main.cpp              # Main application code
│   ├── display_functions.cpp # Display and time functions
│   └── display_functions.h   # Function declarations
├── platformio.ini            # PlatformIO configuration
├── check_env.py             # Environment variable validation
├── wifi_config.template     # WiFi setup instructions
├── .env                     # WiFi credentials (create this)
└── README.md               # This file
```

## Troubleshooting

### Build Errors about Missing Environment Variables

If you get build errors about `WIFI_SSID` or `WIFI_PASSWORD` not being defined:

1. Make sure you created the `.env` file in the project root
2. Check that the `.env` file contains the correct variable names
3. Ensure there are no quotes around the values
4. Restart your PlatformIO environment

### WiFi Connection Issues

1. Verify your WiFi credentials in the `.env` file
2. Check that your ESP32 is within range of your WiFi network
3. Monitor the serial output for connection status
4. Some networks may require additional configuration

### Display Issues

1. Verify your TFT_eSPI configuration matches your hardware
2. Check wiring connections between ESP32 and display
3. Try different rotation settings in `setup()`

## License

This project is based on Adafruit's TFT examples and is provided under the MIT license. 