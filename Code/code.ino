#include <WiFi.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

const char* ssid = "***";
const char* password = "***";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);

#define RST_PIN 23
#define CE_PIN 22
#define DC_PIN 21
#define DIN_PIN 19
#define CLK_PIN 18
#define LIGHT_PIN 5
#define PULSE_SENSOR_PIN 4

Adafruit_PCD8544 display = Adafruit_PCD8544(CE_PIN, DC_PIN, DIN_PIN, CLK_PIN, RST_PIN);

float getTemperature() {
  uint8_t tempSensor = 128;
  return (tempSensor - 32) / 1.8;
}

const char* daysOfWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  timeClient.begin();
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, HIGH);
  pinMode(PULSE_SENSOR_PIN, INPUT);
}

void loop() {
  timeClient.update();
  String timeStr = timeClient.getFormattedTime();
  int dayIndex = timeClient.getDay();
  const char* dayName = daysOfWeek[dayIndex];

  time_t epochTime = timeClient.getEpochTime();
  struct tm *timeInfo = localtime(&epochTime);
  String dateStr = String(timeInfo->tm_mday) + "/" + String(timeInfo->tm_mon + 1) + "/" + String(timeInfo->tm_year + 1900) + " " + dayName;

  int pulseValue = analogRead(PULSE_SENSOR_PIN);
  int bpm = map(pulseValue, 500, 1000, 60, 120);
  float temperature = getTemperature();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  display.setCursor(5, 0);
  display.println(timeStr);
  display.setCursor(5, 10);
  display.println(dateStr);
  display.setCursor(5, 20);
  display.print("BPM: ");
  display.println(bpm);
  display.setCursor(5, 30);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.display();
  delay(1000);
}
