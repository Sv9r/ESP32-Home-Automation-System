#include <Wire.h>
#include <Adafruit_APDS9960.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <ESP_Google_Sheet_Client.h>
#include <time.h>

// WiFi Credentials
#define WIFI_SSID "Shivamrao"
#define WIFI_PASS "shivamrao"

// Sinric Pro Credentials
#define APP_KEY "ae4f2911-a4f9-465b-888d-62825c91f066"
#define APP_SECRET "dd6631ea-5fc5-4974-9b25-15424e44aae2-04468460-7e1b-47f1-b0cf-a0ad07c9f1b7"

// Device IDs for Bulb and LED
#define BULB_DEVICE_ID "6820b4d4dc4a25d5c3cb9780"
#define LED_DEVICE_ID "68238b6d8ed485694c3568c0"

// Pins for Relay and Touch
#define RELAY_PIN_BULB 23
#define RELAY_PIN_LED 25
#define TOUCH_PIN_BULB 14
#define TOUCH_PIN_LED 27
#define TOUCH_THRESHOLD 60

// Google Cloud Credentials (from your JSON)
#define PROJECT_ID "homeautomation-data"
#define CLIENT_EMAIL "homeautomation-data@homeautomation-data.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM =
  "-----BEGIN PRIVATE KEY-----\n"
  "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQD49MQ4O/r0CkWn\n"
  "drTX+uRgOCax1kUu/gIrPw2URTPrhUvxIAg4d3hwM2l1Xor122HlaCPt/o13cxbt\n"
  "FzKkbNxtPWUbkolW++hKHrImnjqWy5mVofMAT5MCuSx9beg0wOWrDustNVJLGDyu\n"
  "XjekV2IT+VTjX4x3HbB3edsfp5PccwoQcHZwb6wsVRGNC/Rl+xbXZmL6fAxlSrai\n"
  "fObyWu9SAeELj3Mm3xGmrpzpYB+yhWs9jA6KmTk/XKpBJbXMEl7MoVNJqBjhvk3X\n"
  "LVCS+/p4Mm0f1Nf4T1LLeh/+5l+XrfYJJ3tHfjSyb7S5IcCs0wA+cgbYJ4kfOSon\n"
  "D6Id1cxbAgMBAAECggEAX929O5xZQCU+ekSbUfoCfEzTFbMZlAVrJ2UR4REe86gy\n"
  "PKllkPzsa2RYzUH5R8BRe8n03/4gUqmGYAkJTsi+vvAf4ZS1f0Hkyrn0RyuyCwRe\n"
  "4YBp9yotRaSxzaEHhFztc8f8Zr28MT+lik2spoOhuGhQOHhAOlQ78l4Bs7hPl1kQ\n"
  "zf3klAK7gg6jqsahxMUEiBMk8uu46KxFTJ6PUndAr5noAScyxfsRugT82287rOjZ\n"
  "XgG8+UHyduk8H5fvuGIHk2kNWJQ9xIRug1uMIL2WavU2hqZv6PWGl2GLv4kjzXXF\n"
  "HNFDbZkcDKB9uObK3Oiy/D27ra8B4ssr+Q2cybvXZQKBgQD95F4g42a8GwasyS/4\n"
  "IUKem2AmRFfGGAEwyatwD3qET4k9verZIv3S8LBbb4y/X4zQnE4AQEEaWWHktOzt\n"
  "nGUViXxHyQdaMcFzFe+ldwfqHMfh5YoOJzIe0rSFByNLluiwAZxTMke65yThtMMc\n"
  "xLVKYQqKrOjwR4hDSSHvtLlGJwKBgQD7Behh/6Y0pgQtdKBT8HTLtXT0OgMvvhzI\n"
  "aH1Zwy+XNroxUwRq+bZJthJ5XYCrunXYNfANrorx9OZBSAhxRNW3NYPQIMlBszyi\n"
  "hLiQRS0y1hJNEQGb0I/XG+9PrY3ws2QiLfcjKha+ZAkkpv0VFjW+IgLfSYMvbDL7\n"
  "LuHTLrX8rQKBgQDWCzg7+9q5zQMUmkyBoPC8LHuj50wkakpeqjaJM9NzRmaKkdkz\n"
  "FCfvNRFJFMMQ9+oMWSn9Vi5Mm3xHRyzTqV0jChSnpFg5ducDMkQmODKI8Zil3lFE\n"
  "W1EK4VJlMD5WVLR45fK3xRTqAkJ6sl0nnMLGIQe1H88F+xhquXYN00u7UwKBgHCl\n"
  "zAz+TrXdso47gh9pDuDDw4I97mQkaoI88dOwVdEEDDZKTPc8rGMTw+KW5UErYeru\n"
  "+ZUKOOwvvIybKL675zRaXZauv+It7w33hWOox4nCq1/OO+ySgZpEvPYCqmnT9yGn\n"
  "gvpSzvYNqlS6Y2RtHRz/4k4VjTtg0NgLx+efVKfZAoGABc2C9magPqzfzRXdgw/9\n"
  "GMORAP4MMgPi74//o3xOLDQOIuvXhY/hlsPqMSMcsAMcjowG4vHzxeO3G3xhoRQh\n"
  "uSOZNUjIksiU8GFyQibf4raynWhR0APo3mknaj3UujR58vSAsqO1m6S7EjFO3bNu\n"
  "IwRymJR9i1ZFNTkFpcfp2UA=\n"
  "-----END PRIVATE KEY-----\n";
const char SPREADSHEET_ID[] = "199Lfua3XMZmVIhHSkoIUCmaiqjV0M6fO8rEp-FazMLo";

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
