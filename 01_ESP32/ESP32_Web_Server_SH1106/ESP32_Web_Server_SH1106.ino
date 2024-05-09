/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/
#include <ESP32-TWAI-CAN.hpp>
// Load Wi-Fi library
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "time.h"

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SH1106Wire.h"

// Default for ESP32
#define CAN_TX    5
#define CAN_RX    4

// Replace with your network credentials
const char* ssid = "Change to your real wifi";
const char* password = "Change to your real PW";
//==================================================
//Getting Date and Time from NTP Server
//==================================================
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 21600;
const int   daylightOffset_sec = 3600;
//==================================================
// Getting Date and Time from NTP Server
//==================================================

//==================================================
//ESP32 HTTP GET with Arduino IDE OpenWeatherMap.org
//==================================================
// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "1c7c9f4be18749cf6f092b255b7b2f72";
// Example:
//String openWeatherMapApiKey = "bd939aa3d23ff33d3c8f5dd1dd435";

// Replace with your country code and city
String city = "Ho Chi Minh";
String countryCode = "VN";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;
unsigned long timerDelay_100ms = 100;

String jsonBuffer;
//==================================================
//ESP32 HTTP GET with Arduino IDE OpenWeatherMap.org
//==================================================

// Set web server port number to 80
WiFiServer server(80);
SH1106Wire display(0x3c, SDA, SCL);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


// // Simple sketch that querries OBD2 over CAN for coolant temperature
// Showcasing simple use of ESP32-TWAI-CAN library driver.
CanFrame rxFrame;

void sendObdFrame(uint8_t obdId) {
  CanFrame obdFrame = { 0 };
  obdFrame.identifier = 0x7DF; // Default OBD2 address;
  obdFrame.extd = 0;
  obdFrame.data_length_code = 8;
  obdFrame.data[0] = 2;
  obdFrame.data[1] = 1;
  obdFrame.data[2] = obdId;
  obdFrame.data[3] = 0xAA;    // Best to use 0xAA (0b10101010) instead of 0
  obdFrame.data[4] = 0xAA;    // CAN works better this way as it needs
  obdFrame.data[5] = 0xAA;    // to avoid bit-stuffing
  obdFrame.data[6] = 0xAA;
  obdFrame.data[7] = 0xAA;
    // Accepts both pointers and references 
    ESP32Can.writeFrame(obdFrame);  // timeout defaults to 1 ms
}

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //
  display.init();
  display.flipScreenVertically();
  display.setContrast(255);
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  // Align text vertical/horizontal center
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_10);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "User control:\n" + WiFi.localIP().toString());

//==================================================
// Getting Date and Time from NTP Server
//==================================================
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();

  // Set pins
  ESP32Can.setPins(CAN_TX, CAN_RX);
  
    // You can set custom size for the queues - those are default
    ESP32Can.setRxQueueSize(5);
  ESP32Can.setTxQueueSize(5);

    // .setSpeed() and .begin() functions require to use TwaiSpeed enum,
    // but you can easily convert it from numerical value using .convertSpeed()
    ESP32Can.setSpeed(ESP32Can.convertSpeed(500));

    // You can also just use .begin()..
    if(ESP32Can.begin()) {
        Serial.println("CAN bus started!");
    } else {
        Serial.println("CAN bus failed!");
    }

    // or override everything in one command;
    // It is also safe to use .begin() without .end() as it calls it internally
    if(ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
        Serial.println("CAN bus started!");
    } else {
        Serial.println("CAN bus failed!");
    }
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  WifiRSSI_draw_7x8(128-8,2,WiFi.RSSI());
  // Time
  printLocalTime(0,54);
  
  // RSSI end
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
              display.setColor(BLACK);
              display.fillRect(0,23,128,20);
              display.setColor(WHITE);
              display.drawString(display.getWidth() / 2, display.getHeight() / 2, "User control:\n" + output26State);
              display.display();
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
              display.setColor(BLACK);
              display.fillRect(0,23,128,20);
              display.setColor(WHITE);
              display.drawString(display.getWidth() / 2, display.getHeight() / 2, "User control:\n" + output26State);
              display.display();
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
              display.setColor(BLACK);
              display.fillRect(0,23,128,20);
              display.setColor(WHITE);
              display.drawString(display.getWidth() / 2, display.getHeight() / 2, "User control:\n" + output27State);
              display.display();
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
              display.setColor(BLACK);
              display.fillRect(0,23,128,20);
              display.setColor(WHITE);
              display.drawString(display.getWidth() / 2, display.getHeight() / 2, "User control:\n" + output27State);
              display.display();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");

    //

  }
      // Send an HTTP GET request
  if ((millis() - lastTime) > 1000) {
    getWeatherInfo();
    sendObdFrame(5); // For coolant temperature
    lastTime = millis();
    

  }
      // You can set custom timeout, default is 1000
    if(ESP32Can.readFrame(rxFrame, 1000)) {
        // Comment out if too many frames
        Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
        if(rxFrame.identifier == 0x7E8) {   // Standard OBD2 frame responce ID
            Serial.printf("Collant temp: %3d°C \r\n", rxFrame.data[3] - 40); // Convert to °C
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0,40,"0x7E8 " + String(rxFrame.data[3] - 40) + "DegC");
        }
    }
}

void getWeatherInfo()
{
  // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_10);
        display.drawString(0,0,"Parsing input failed!");
        return;
      }
        //erase place
      display.setColor(BLACK);
      display.fillRect(0,0,120,30);
      display.setColor(WHITE);
      // Align text vertical/horizontal center
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      display.drawString(0,0,"Temp: " + JSON.stringify(myObject["main"]["temp"]));
      display.drawString(0,10,"Pressure: " + JSON.stringify(myObject["main"]["pressure"]));
      display.drawString(0,20,"Humidity: " + JSON.stringify(myObject["main"]["humidity"]));
    }
    else {
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      display.drawString(0,0,"WiFi Disconnected");
    }
    display.display();
}

// Getting Date and Time from NTP Server
void printLocalTime(int16_t x, int16_t y){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
      //erase place
    display.setColor(BLACK);
    display.fillRect(x,y,120,10);
    display.setColor(WHITE);
      // Align text vertical/horizontal center
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  char buffer[80];
  strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &timeinfo);
  display.drawString(x,y,buffer);

  display.display();
}
// Getting Date and Time from NTP Server


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void WifiRSSI_draw_7x8(int16_t x0, int16_t y0, int16_t RSSI_value) {
  // Check place
  if((x0 >= 0) && (x0 <= (127-7)) && (y0 >= 0) && (y0<=(63-8)))
  {
    //erase place
    display.setColor(BLACK);
    display.fillRect(x0,y0,7,8);
    display.setColor(WHITE);
  // draw frame
  display.setPixel(x0+0,y0+(7-1));
  display.setPixel(x0+0,y0+(7-0));
  display.setPixel(x0+2,y0+(7-3));
  display.setPixel(x0+2,y0+(7-0));
  display.setPixel(x0+4,y0+(7-5));
  display.setPixel(x0+4,y0+(7-0));
  display.setPixel(x0+6,y0+(7-7));
  display.setPixel(x0+6,y0+(7-0));

  int RSSI_ae = (120+RSSI_value)/6;
  if(RSSI_ae<=2)
  {
  display.drawVerticalLine(x0+0,y0+(7-1),RSSI_ae);
  }
  else if(RSSI_ae<=6)
  {
  display.drawVerticalLine(x0+0,y0+(7-1),2);
  display.drawVerticalLine(x0+2,y0+8-(RSSI_ae-2),RSSI_ae-2);
  }
  else if(RSSI_ae<=12)
  {
  display.drawVerticalLine(x0+0,y0+(7-1),2);
  display.drawVerticalLine(x0+2,y0+(7-3),4);
  display.drawVerticalLine(x0+4,y0+8-(RSSI_ae-6),RSSI_ae-6);
  }
  else
  {
  display.drawVerticalLine(x0+0,y0+(7-1),2);
  display.drawVerticalLine(x0+2,y0+(7-3),4);
  display.drawVerticalLine(x0+4,y0+(7-5),6);
  display.drawVerticalLine(x0+6,y0+8-(RSSI_ae-12),RSSI_ae-12);
  }
  display.display();
  }
}
