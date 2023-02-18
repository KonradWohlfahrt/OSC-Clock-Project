/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, January 18, 2023.
  Released into the public domain.
*/

/*
  --- --- --- --- --- --- INCLUDE --- --- --- --- --- ---
*/
#include <MD_MAX72xx.h>
#include <DHT.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <WiFiClient.h>

#include "serverhtml.h"


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

#define LONGPRESSTIME 1000

/* --- DHT11 --- */
#define DHTPIN 4 
#define DHTTYPE DHT11


/*
  --- --- --- --- --- --- VARIABLES --- --- --- --- --- ---
*/
/* --- SCROLL EFFECT ---*/

int scrollTextSpeed = 50; // from 10 to 250 ms, default = 50

int currentCharLength = 0;
int currentCharIndex = 0;
int currentTextIndex = 0;
byte currentCharBuffer[8];
String currentDisplayText = "";

String ipText;
String customText = "unset text";

int brightness = 3;

/*
  --- --- --- --- --- --- STATIC VARIABLES --- --- --- --- --- ---
*/
/* --- WIFI --- */

const String ssid = "Matrix Display"; // maximum 31
const String password = "123456789"; // minimum 8, maximum 63 

/* --- TEXT --- */
const String createAPText = "creating access point...";

const String defaultTexts[5] = {
  "text 1",
  "text 2",
  "text 3",
  "text 4",
  "text 5"
};