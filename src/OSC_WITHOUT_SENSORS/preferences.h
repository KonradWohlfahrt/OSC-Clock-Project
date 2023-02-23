/*
  --- --- --- --- --- --- INCLUDE --- --- --- --- --- ---
*/
// display
#include <MD_MAX72xx.h>
// eeprom to save api keys, wifi data
#include <EEPROM.h>

// wifi libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

// library to get the time from the internet
#include <NTPClient.h>

// library to phrase the data from openweathermap.org
#include <ArduinoJson.h>

// custom time library
#include "DonutStudioMillisTime.h"

// html file
#include "accesspointhtml.h"


/*
  --- --- --- --- --- --- DEFINE --- --- --- --- --- ---
*/
/* --- DISPLAY --- */
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 14 // SCK
#define DATA_PIN 13 // MOSI
#define CS_PIN 15 // SS

/* --- PIEZO-BUZZER --- */
#define BUZZER 0

/* --- BUTTON --- */
#define LONGPRESSTIME 1000

/* --- WIFI --- */
#define WIFICONNECTIONTIMEOUT 32

/*
  --- --- --- --- --- --- LIBRARY VARIABLES --- --- --- --- --- ---
*/
ESP8266WebServer server = ESP8266WebServer(80);

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MillisTime clkTime = MillisTime(12, 0, 0); 

WiFiUDP ntpUDP;
// ntp servers, 0 = +00:00, 3600000 = update after 1 hour
NTPClient timeClient(ntpUDP, "COUNTRYCODE.pool.ntp.org", 0, 3600000);
unsigned long timeUpdateInterval = 3600000; // one hour



WiFiClient client;
String cityName = "CITYNAME,COUNTRYCODE";
unsigned long weatherUpdateInterval = 60000; // one minute


String ssid = ""; // limited to 32 chars
String password = ""; // limited to 32 chars
String weatherAPIKey = ""; // limited to 64 chars


/*
  --- --- --- --- --- --- SETTINGS --- --- --- --- --- ---
*/
// the timeouttime in milliseconds
int timeoutTimes[] = { 5000, 10000, 15000, 20000, 25000, 30000 };

// time in ms, length has to be even: alternating between pausing and playing, beginning with pause
int alarmSoundTime[] = { 200, 100, 200, 100, 200, 50, 100, 50, 100, 50 };
int alarmSoundLength = 10;

/*
  --- --- --- --- --- --- ICONS AND TEXT --- --- --- --- --- ---
*/
/* --- MAIN --- */
byte weatherIcon[8] = { 24, 188, 62, 94, 28, 190, 62, 28 };
String weatherText = "Weather";
String weatherResultText = "no data";

byte timerIcon[8] = { 0, 48, 74, 182, 166, 74, 48, 0 };
byte timerActiveIcon[8] { 3, 49, 74, 182, 166, 74, 49, 3 };
String timerText = "Timer";
String timerDoneText = "end";

byte settingsIcon[8] = { 192, 224, 112, 62, 31, 25, 24, 12 };
String settingsText = "Settings";

/* --- SETTINGS --- */
byte returnIcon[8] = { 8, 28, 62, 8, 8, 72, 48, 0 };
String returnText = "Back";

byte timeIcon[8] = { 60, 66, 129, 157, 145, 129, 66, 60 };
String timeText = "Time";

byte alarmIcon[8] = { 0, 16, 60, 190, 190, 60, 16, 0 };
String alarmText = "Alarm";
String alarmActiveText = "wake";

byte brightnessIcon[8] = { 90, 129, 24, 189, 189, 24, 129, 90 };

byte timeoutIcon[8] = { 196, 164, 148, 140, 50, 42, 38, 0 };

byte wifiIcon[8] = { 213, 213, 21, 229, 9, 242, 4, 248 };
String wifiText = "WIFI";

/*
  --- --- --- --- --- --- WIFI TEXT --- --- --- --- --- ---
*/
int scrollTextSpeed = 40;
String wifiConnectionText = "Connect to WiFi...";
String failedConnectionText = "Failed!";

String loadingText = "Loading";

String createAccessPointText = "Offlinemode: press button!";
String apNameText = "Access point name: ";