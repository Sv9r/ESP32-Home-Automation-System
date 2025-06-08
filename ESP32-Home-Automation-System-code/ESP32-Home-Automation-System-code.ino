#include <Wire.h>
#include <Adafruit_APDS9960.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <ESP_Google_Sheet_Client.h>
#include <time.h>

// WiFi Credentials
#define WIFI_SSID "YOUR-SSID"
#define WIFI_PASS "YOUR-PASSWORD"

// Sinric Pro Credentials
#define APP_KEY "YOUR-APPKEY"
#define APP_SECRET "YOUR-APP-SECRET"

// Device IDs for Bulb and LED
#define BULB_DEVICE_ID "YOUR-BULB-ID"
#define LED_DEVICE_ID "YOUR-LED-ID"

// Pins for Relay and Touch
#define RELAY_PIN_BULB 23
#define RELAY_PIN_LED 25
#define TOUCH_PIN_BULB 14
#define TOUCH_PIN_LED 27
#define TOUCH_THRESHOLD 60

// Google Cloud Credentials (from your JSON)
#define PROJECT_ID "YOUR-PROJECT-ID"
#define CLIENT_EMAIL "YOUR-CLIENT-EMAIL"
const char PRIVATE_KEY[] PROGMEM =
  "-----BEGIN PRIVATE KEY-----\n"
  " "
  "-----END PRIVATE KEY-----\n";
const char SPREADSHEET_ID[] = "YOUR-SPREADSHEET-ID";

// NTP Server for IST
const char* ntpServer = "in.pool.ntp.org";
const long gmtOffset_sec = 19800;  // 5:30 in seconds
const int daylightOffset_sec = 0;

// Google Sheets Client
// ESP_Google_Sheet_Client GSheet;

// Device states
bool bulbOn = false;
bool ledOn = false;

// Gesture Sensor
Adafruit_APDS9960 apds;

// SinricPro Device References
SinricProSwitch& bulbSwitch = SinricPro[BULB_DEVICE_ID];
SinricProSwitch& ledSwitch = SinricPro[LED_DEVICE_ID];

// Log event to Google Sheets, with source
void logEventToSheet(const String& device, bool state, const String& source) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  char timeStr[25];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

  FirebaseJson valueRange;
  valueRange.add("majorDimension", "COLUMNS");
  valueRange.set("values/[0]/[0]", timeStr);               // Timestamp
  valueRange.set("values/[1]/[0]", device);                // Device
  valueRange.set("values/[2]/[0]", state ? "ON" : "OFF");  // State
  valueRange.set("values/[3]/[0]", source);                // Source

  FirebaseJson response;
  GSheet.values.append(&response, SPREADSHEET_ID, "Sheet1!A1", &valueRange);
}

// Sinric Callback function (Voice/Cloud)
bool onPowerState(const String& deviceId, bool& state) {
  String device, source = "Voice";
  if (deviceId == LED_DEVICE_ID) {
    device = "LED";
    ledOn = state;
    digitalWrite(RELAY_PIN_LED, ledOn ? HIGH : LOW);
  } else if (deviceId == BULB_DEVICE_ID) {
    device = "Bulb";
    bulbOn = state;
    digitalWrite(RELAY_PIN_BULB, bulbOn ? LOW : HIGH);
  }
  logEventToSheet(device, state, source);
  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32 Default I2C Pins (SDA=21, SCL=22)

  if (!apds.begin()) {
    while (1)
      ;  // APDS9960 not found
  }
  apds.enableGesture(true);

  pinMode(RELAY_PIN_BULB, OUTPUT);
  pinMode(RELAY_PIN_LED, OUTPUT);
  digitalWrite(RELAY_PIN_BULB, LOW);
  digitalWrite(RELAY_PIN_LED, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Sinric Pro
  SinricPro.begin(APP_KEY, APP_SECRET);
  bulbSwitch.onPowerState(onPowerState);
  ledSwitch.onPowerState(onPowerState);

  // NTP for IST
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  int retries = 0;
  while (!getLocalTime(&timeinfo) && retries < 20) {
    delay(100);
    retries++;
  }

  // Google Sheets Client
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}

void loop() {
  SinricPro.handle();

  // Gesture control
  uint8_t gesture = apds.readGesture();
  if (gesture) {
    String device, eventSource = "Gesture";
    switch (gesture) {
      case APDS9960_UP:
        ledOn = !ledOn;
        digitalWrite(RELAY_PIN_LED, ledOn ? LOW : HIGH);
        ledSwitch.sendPowerStateEvent(ledOn);
        device = "LED";
        logEventToSheet(device, ledOn, eventSource);
        break;
      case APDS9960_DOWN:
        ledOn = !ledOn;
        digitalWrite(RELAY_PIN_LED, ledOn ? LOW : HIGH);
        ledSwitch.sendPowerStateEvent(ledOn);
        device = "LED";
        logEventToSheet(device, ledOn, eventSource);
        break;
      case APDS9960_LEFT:
        bulbOn = !bulbOn;
        digitalWrite(RELAY_PIN_BULB, bulbOn ? LOW : HIGH);
        bulbSwitch.sendPowerStateEvent(bulbOn);
        device = "Bulb";
        logEventToSheet(device, bulbOn, eventSource);
        break;
      case APDS9960_RIGHT:
        bulbOn = !bulbOn;
        digitalWrite(RELAY_PIN_BULB, bulbOn ? LOW : HIGH);
        bulbSwitch.sendPowerStateEvent(bulbOn);
        device = "Bulb";
        logEventToSheet(device, bulbOn, eventSource);
        break;
    }
    delay(100);  // Debounce
  }

  // Touch control
  if (touchRead(TOUCH_PIN_BULB) < TOUCH_THRESHOLD) {
    bulbOn = !bulbOn;
    digitalWrite(RELAY_PIN_BULB, bulbOn ? LOW : HIGH);
    bulbSwitch.sendPowerStateEvent(bulbOn);
    logEventToSheet("Bulb", bulbOn, "Touch");
  }

  if (touchRead(TOUCH_PIN_LED) < TOUCH_THRESHOLD) {
    ledOn = !ledOn;
    digitalWrite(RELAY_PIN_LED, ledOn ? LOW : HIGH);
    ledSwitch.sendPowerStateEvent(ledOn);
    logEventToSheet("LED", ledOn, "Touch");
  }
}
