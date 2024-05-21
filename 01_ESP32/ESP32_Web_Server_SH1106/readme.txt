# ESP32 CAN Bus and Web Server Project

This project demonstrates how to use an ESP32 microcontroller to create a system integrating a CAN (Controller Area Network) bus interface and a web server. The ESP32 gathers data from OpenWeatherMap and an OBD2 system, displaying the information on an SH1106 OLED screen. It also hosts a web server for remote control of GPIO pins.

## Components

- ESP32 Development Board
- SH1106 OLED Display
- CAN Transceiver Module

## Libraries

- `WiFi.h` for Wi-Fi connectivity
- `HTTPClient.h` for HTTP requests
- `Arduino_JSON.h` for JSON parsing
- `ESP32-TWAI-CAN.hpp` for CAN bus communication
- `Wire.h` and `SH1106Wire.h` for the OLED display

## Setup

### Wi-Fi Configuration

```cpp
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";
```

### NTP Server Configuration

```cpp
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 21600;
const int daylightOffset_sec = 3600;
```

### OpenWeatherMap API

```cpp
String openWeatherMapApiKey = "Your_API_Key";
String city = "Your_City";
String countryCode = "Your_Country_Code";
```

### GPIO Pins

```cpp
const int output26 = 26;
const int output27 = 27;
```

## Initialization in `setup()`

1. Initialize Serial Communication
2. Configure GPIO Pins
3. Connect to Wi-Fi
4. Initialize OLED Display
5. Start Web Server
6. Configure CAN Bus

## Main Loop

1. Handle Client Connections
2. Send and Receive CAN Bus Frames
3. Update Weather Information
4. Display Data on OLED

## Key Functions

### Send CAN Frames

```cpp
void sendObdFrame(int pid) {
  ESP32CanFrame txFrame;
  txFrame.identifier = 0x7DF;
  txFrame.dataLength = 8;
  txFrame.data[0] = 0x02;
  txFrame.data[1] = 0x01;
  txFrame.data[2] = pid;
  txFrame.data[3] = 0x55;
  txFrame.data[4] = 0x55;
  txFrame.data[5] = 0x55;
  txFrame.data[6] = 0x55;
  txFrame.data[7] = 0x55;
  ESP32Can.sendFrame(txFrame);
}
```

### HTTP GET Request

```cpp
String httpGETRequest(const char* serverName) {
  HTTPClient http;
  http.begin(serverName);
  int httpResponseCode = http.GET();
  
  String payload = "{}";
  if (httpResponseCode > 0) {
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
  return payload;
}
```

## Conclusion

This project showcases the ESP32's capability to integrate IoT and automotive applications, providing a robust solution for remote monitoring and control with real-time weather updates and CAN bus data displayed on an OLED screen.
