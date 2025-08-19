/**************************************************************************
  ESP32-S3 LCD Cube Clock
  
  Converted from Particle Argon to Adafruit QT Py ESP32-S3
  
  This displays a digital clock on an ST7789-based TFT display.
  Connects to WiFi and synchronizes time using NTP.
  
  Pin Connections for QT Py ESP32-S3:
  - TFT CS  -> Pin 7
  - TFT RST -> Pin 8  
  - TFT DC  -> Pin 9
  - TFT SCK -> SCK (Hardware SPI)
  - TFT MOSI -> MOSI (Hardware SPI)
 **************************************************************************/

#include <TFT_eSPI.h> // Graphics and font library for ST7789 driver chip
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// NTP server settings
const char* ntpServer = "pool.ntp.org";

// Timezone strings for automatic DST handling
// Format: "STD offset DST [offset],start[/time],end[/time]"
// Common US timezones (automatically handles DST):
// Pacific:  "PST8PDT,M3.2.0,M11.1.0"
// Mountain: "MST7MDT,M3.2.0,M11.1.0"  
// Central:  "CST6CDT,M3.2.0,M11.1.0"
// Eastern:  "EST5EDT,M3.2.0,M11.1.0"
// Arizona (no DST): "MST7"
// M3.2.0 = 2nd Sunday in March, M11.1.0 = 1st Sunday in November
const char* timezone_str = "PST8PDT,M3.2.0,M11.1.0";

// Pin definitions are in User_Setup.h for TFT_eSPI
// TFT_BL is defined in User_Setup.h
#ifndef TFT_BL
  #define TFT_BL 20  // Backlight pin
#endif

// Create display object
TFT_eSPI tft = TFT_eSPI();

// Global variables for time display
String _dayOfWeek;
String _date; 
String _time;
struct tm timeinfo;

// Helper function to get day of week abbreviation
String getDayOfWeekAbbr(int wday) {
  const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  return String(days[wday]);
}

// Helper function to format time in 12-hour format
String formatTime12Hour(struct tm *timeinfo) {
  int hour = timeinfo->tm_hour;
  
  if (hour == 0) hour = 12;      // Convert midnight to 12 AM
  else if (hour > 12) hour -= 12; // Convert PM hours to 12-hour format
  
  char timeStr[10];
  sprintf(timeStr, "%d:%02d", hour, timeinfo->tm_min);
  return String(timeStr);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.println("Connecting to WiFi");
  
  int attempts = 0;
  tft.setCursor(0, 0);
  tft.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    Serial.print(".");
    delay(1000);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi");
  }
}

void setupTime() {
  // Configure time with NTP server first
  configTime(0, 0, ntpServer);
  
  // Then set timezone string for automatic DST handling
  setenv("TZ", timezone_str, 1);
  tzset();
  
  tft.println("Waiting for NTP time sync");
  Serial.print("Waiting for NTP time sync");
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 10) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (getLocalTime(&timeinfo)) {
    Serial.println();
    Serial.println("Time synchronized with NTP server");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    Serial.print("Timezone: ");
    Serial.println(timezone_str);
    Serial.print("DST: ");
    Serial.println(timeinfo.tm_isdst ? "Yes" : "No");
  } else {
    Serial.println();
    Serial.println("Failed to obtain time from NTP server");
  }
}

void printClock() {
  // Set backlight pwm signal to 1 between the hourse of 10pm and 7am, and 255 for 7am to 10pm
  // if (timeinfo.tm_hour >= 22 || timeinfo.tm_hour < 7) {
  //   analogWrite(TFT_BL, 1);
  // } else {
  //   analogWrite(TFT_BL, 255);
  // }
  analogWrite(TFT_BL, 1);

  /**
   * Center the clock text on 240x240 screen
   * Each text row is vertically centered at 120px
   * 
   * Row 1: text size 3 (18px high)
   * Row 2: text size 2 (12px high) 
   * Row 3: text size 6 (36px high)
   * Total height: ~66px, starting at y=87 for centering
   */
  tft.setRotation(4);      // Use 4 for Prism version, 2 for NoPrism version
  tft.invertDisplay(1);    // 1 = invert colors, 0 = normal
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 87);
  
  // Get current local time
  if (getLocalTime(&timeinfo)) {
    _dayOfWeek = getDayOfWeekAbbr(timeinfo.tm_wday);
    _date = String(timeinfo.tm_mday);
    _time = formatTime12Hour(&timeinfo);
  } else {
    // Fallback if time is not available
    _dayOfWeek = "---";
    _date = "--";
    _time = "--:--";
  }
  
  // Display day and date
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(3);
  tft.print(_dayOfWeek);
  tft.print(" ");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(_date);
  
  // Small spacing
  tft.setTextSize(2);
  tft.println(" ");
  
  // Display time (largest)
  tft.setTextSize(6);
  tft.println(_time);
  
  tft.setCursor(0, 0);
}

void setup(void) {
  Serial.begin(115200);
  delay(500);

  
  Serial.println(F("ESP32-S3 LCD Cube Clock Starting..."));



  // Initialize display
  tft.init();
  
  // Set rotation if needed (0-3)
  // tft.setRotation(0);
  
  // Mirror display on Y axis by setting MX bit in MADCTL register
  // MADCTL bits: MY MX MV ML BGR MH x x
  // MX = 1 mirrors horizontally (reflects on Y axis)
  // tft.writecommand(0x36);  // MADCTL register
  // tft.writedata(0x40);     // MX bit set (0x40 = mirror on Y axis)
  
  // The TFT_eSPI library handles the ST7789 initialization including
  // any necessary offsets and MADCTL settings based on User_Setup.h

  Serial.println(F("Display Initialized"));

  
  // Clear screen
  tft.fillScreen(TFT_BLACK);
  
  // Connect to WiFi
  connectToWiFi();
  
  // Setup NTP time synchronization
  if (WiFi.status() == WL_CONNECTED) {
    setupTime();
  }
}

void loop() {
  Serial.println("Updating clock display...");
  printClock();
  delay(1000);
} 

