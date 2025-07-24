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

// #include <Adafruit_GFX.h>    // Core graphics library
// #include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// NTP server settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -7 * 3600; // GMT-7 (Mountain Time) - CHANGE TO YOUR TIMEZONE
const int daylightOffset_sec = 3600;   // Daylight saving time offset

// Pin definitions for QT Py ESP32-S3
#define TFT_CS         8  // Chip select
#define TFT_RST       17  // Reset  
#define TFT_DC         9  // Data/Command
#define TFT_BL         18  // Backlight

// Create display object for ST7789 (240x240) using hardware SPI
Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, TFT_CS, TFT_DC, TFT_RST);

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
  // Configure time with NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
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
  } else {
    Serial.println();
    Serial.println("Failed to obtain time from NTP server");
  }
}

void printClock() {
  // Set backlight pwm signal to 1 between the hourse of 10pm and 7am, and 255 for 7am to 10pm
  if (timeinfo.tm_hour >= 22 || timeinfo.tm_hour < 7) {
    analogWrite(TFT_BL, 1);
  } else {
    analogWrite(TFT_BL, 255);
  }

  /**
   * Center the clock text on 240x240 screen
   * Each text row is vertically centered at 120px
   * 
   * Row 1: text size 3 (18px high)
   * Row 2: text size 2 (12px high) 
   * Row 3: text size 6 (36px high)
   * Total height: ~66px, starting at y=87 for centering
   */
  tft.fillScreen(ST77XX_BLACK);
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
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(3);
  tft.print(_dayOfWeek);
  tft.print(" ");
  tft.setTextColor(ST77XX_WHITE);
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



  // Initialize the 240x240 ST7789 display
  tft.init(240, 240);

  // Set display orientation with custom MADCTL (after initialization)
  uint8_t madctl = ST77XX_MADCTL_MX;
  tft.sendCommand(ST77XX_MADCTL, &madctl, 1);

  // If row address order is swapped (MX bit set), we need to set vertical scroll offset
  // The ST7789 has 320x240 memory but only shows 240x240, so we need 80 pixel offset
  if (madctl) {
      Serial.println("Setting vertical scroll offset");
      uint8_t scroll_data[2] = {0, 80}; // Set vertical scroll start address to 80
      tft.sendCommand(0x37, scroll_data, 2); // 0x37 = VSCSAD (Vertical Scroll Start Address)
  }

  // Optional: Set custom SPI speed (default is usually fine)
  // tft.setSPISpeed(40000000);

  Serial.println(F("Display Initialized"));

  
  // Clear screen
  tft.fillScreen(ST77XX_BLACK);
  
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
  delay(60000);
} 

