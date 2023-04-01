/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

/*
  --- --- --- LIBRARIES --- --- ---
*/
// dot matrix display
#include <MD_MAX72xx.h>
// dht11 sensor
#include <DHT.h>
// eeprom of the chip
#include <EEPROM.h>

// wifi libraries for the esp8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

// network time protocol client library
#include <NTPClient.h>

// json phrasing library (5.13.4)
#include <ArduinoJson.h>

// fast fourier transform library
#include <arduinoFFT.h>

// custom libraries: time, stopwatch, timer
#include <DonutStudioMillisTime.h>
#include <DonutStudioStopwatch.h>
#include <DonutStudioTimer.h>

// file with the html code
#include "accesspointhtml.h"


/*
  --- --- --- DEFINE --- --- ---
*/
/* --- DISPLAY --- */
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 14 // SCK
#define DATA_PIN 13 // MOSI
#define CS_PIN 15 // SS

/* --- PINS --- */
#define BUZZER 0
#define MIC A0
#define DHTPIN 4

/* --- FFT --- */
// Must be a power of 2, a higher values will increase the accuracity but also processing time
#define SAMPLES 64
// pixel on the x-axis
#define XRES 32
// pixel on the y-axis
#define YRES 8 

// the type of the sensor
#define DHTTYPE DHT11
// after how many milliseconds is a press counted as a long press
#define LONGPRESSTIME 1000
// timeout time for the wifi connection: 32 * 250ms = 8000ms = 8s
#define WIFICONNECTIONTIMEOUT 32
// timeout time for the access point in milliseconds
#define ACCESPOINTCONFIRMATIONTIMEOUT 20000
// timeoutt time for the openweathermap api request
#define WEATHERAPIREQUESTTIMEOUT 5000
// in the sleepmode the clock only checks the alarm, timer and time-update only every 2 seconds
#define SLEEPMODECHECKINTERVAL 2000



/*
  --- --- --- LIBRARY VARIABLES --- --- ---
*/

// display
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// dht11 sensor
DHT dht(DHTPIN, DHTTYPE);

/* --- NTPCLIENT --- */
WiFiUDP ntpUDP;
const unsigned long timeUpdateInterval = 3600000; // (in milliseconds), 3600000 = 1h
// "countryID.pool.ntp.org" -> ntp server url, timeoffset in seconds
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 3600, timeUpdateInterval);

/* --- WEATHERAPI --- */
WiFiClient weatherClient;
String cityName = "cityName,countryID";
const unsigned long weatherUpdateInterval = 1800000; // (in milliseconds), 1800000 = 30min

// time library
MillisTime clkTime = MillisTime(12, 0, 0); 
// stopwatch library
Stopwatch stpWatch = Stopwatch(); 
// timer library: start at 00h:15m:00s:00ms
Timer timer = Timer(0, 15);


/*
  --- --- --- CONSTANT VARIABLES --- --- ---
*/
/* --- WIFI TEXT --- */

const String tryConnectionText = "Connecting...";
const String failedText = "Failed!";
String createAccessPointText = "Press a key for offlinemode";
String reenterDataText = "Press a key to cancel";
String accessPointExplanation = "Connect your device with 'OSC-AP' and open this website in your Browser: ";
const String noNetworksFoundText = "no networks found";
const String dataSavedText = "The data was saved and the clock will restart. You can close this page now!";

const String loadingText = "Loading";
const String noWeatherDataText = "no data";

const String wiFiInfoText1 = "SSID: ";
const String wiFiInfoText2 = " +++ IP: ";
String wifiInfoText3 = " +++ Signalstrength: ";


/* --- OTHER --- */

// the refresh time of the display
const int refreshTime = 500;

// the timeouttime in milliseconds
const int timeoutTimes[] = { 5000, 10000, 15000, 20000, 25000, 30000 };

// time in ms, length has to be even: alternating between pausing and playing, beginning with pause
const int alarmSoundTime[] = { 200, 100, 200, 100, 200, 50, 100, 50, 100, 50 };
const int alarmSoundLength = 10;

// columns to be activated by the frequency data (0 - 7)
const int styleArray[8] = { 128, 192, 224, 240, 248, 252, 254, 255 };


/* --- ICONS AND TEXT --- */

byte audioVisualizerIcon[8] = { 96, 240, 240, 127, 6, 12, 24, 0 };
const String audioVisualizerText = "Audio Visualizer";
byte amplitudeIcon[8] = { 24, 12, 6, 12, 24, 48, 96, 48 }; 
byte noiseIcon[8] = { 24, 60, 60, 126, 24, 36, 54, 18 };

byte weatherIcon[8] = { 24, 188, 62, 94, 28, 190, 62, 28 };
const String weatherText = "Weather";

byte timerIcon[8] = { 0, 195, 165, 153, 153, 165, 195, 0 };
const String timerText = "Timer";
const String timerDoneText = "End";

byte stopwatchIcon[8] = { 0, 48, 74, 182, 166, 74, 48, 0 };
const String stopwatchText = "Stopwatch";

byte settingsIcon[8] = { 192, 224, 112, 62, 31, 25, 24, 12 };
const String settingsText = "Settings";

byte returnIcon[8] = { 8, 28, 62, 8, 8, 72, 48, 0 };
String returnText = "Back";

byte timeIcon[8] = { 60, 66, 129, 157, 145, 129, 66, 60 };
const String timeText = "Time";

byte alarmIcon[8] = { 0, 16, 60, 190, 190, 60, 16, 0 };
const String alarmText = "Alarm";
const String alarmActiveText = "Wake-up";

byte brightnessDayIcon[8] = { 153, 66, 24, 189, 189, 24, 66, 153 };
byte brightnessNightIcon[8] = { 60, 66, 129, 141, 147, 144, 72, 56 };

byte dayTimeIcon[8] { 60, 126, 255, 255, 129, 129, 66, 60 };
const String dayTimeText = "Nightmode";

byte microphoneSensivityIcon[8] = { 0, 48, 174, 239, 239, 174, 48, 0 };
String microphoneSensivityText = "Microphonesensivity";

byte timeoutIcon[8] = { 196, 164, 148, 140, 50, 42, 38, 0 };

const String textSpeedText = "Textspeed";

byte wifiInfoIcon[8] = { 213, 213, 21, 229, 9, 242, 4, 248 };
const String wifiInfoText = "Connection info";

byte wifiDataIcon[8] = { 255, 129, 151, 149, 151, 130, 252, 0 };
const String wifiDataText = "Data entry";

/*
  --- --- --- NON-CONSTANT VARIABLES --- --- ---
*/

String ssid = ""; // limited to 32 chars
String password = ""; // limited to 32 chars
String weatherAPIKey = ""; // limited to 64 chars

// the timestamp for the last refresh of the display
unsigned long lastRefreshTimestamp;
// the timestamp for the last interaction with the clock
unsigned long lastInteractionTimestamp;
// the timestamp for the text shift of the display
unsigned long lastShiftTimestamp;

// the results from the weather api call
String weatherResultText = "";

/* --- SETTINGS --- */
// the time it takes to shift the text by one pixel
int textScrollTime = 30;
// when should the clock go into the sleepmode? (in ms)
int timeout;
// the index of the timeout array, currently selected timeout
int timeoutIndex = 2;

// the current brightness of the display: 0-15 (set to 16 to force an update)
int currentBrightness = 16;
// the brightness at daytime
int brightnessDay;
// the brightness at nighttime
int brightnessNight;

// filters out all frequencies that are below this value
int noise = 300;
// will transform the frequency to the bars, a higher value means lower bar heights
int amplitude = 500;

// at what time should the night start
int nightStart = 20;
// at what time should the night end
int nightEnd = 7;

// the sensivity of the microphone (when should the clock wake up from the sleepmode?) 35-55
int microphoneSensivity = 45;