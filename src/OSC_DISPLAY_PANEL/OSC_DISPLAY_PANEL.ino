/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, January 18, 2023.
  Released into the public domain.
*/


#include "prefs.h"

/*
  --- --- --- --- --- --- VARIABLES --- --- --- --- --- ---
*/

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server = ESP8266WebServer(80);

unsigned long previousRefreshTime = 0; 

bool displayActive = false;
int textIndex = 0; // 0=ip, 1-5=default text, 6=custom text

/*
  --- --- --- --- --- --- METHODS --- --- --- --- --- ---
*/
/*
  --- --- SETTING METHODS --- ---
*/
/* --- BUTTONS --- */
bool upButton() { return digitalRead(12) == 0; }
bool downButton() { return digitalRead(5) == 0; }
bool modeButton() { return digitalRead(3) == 0; }
bool anyButton() { return upButton() || downButton() || modeButton(); }
bool anySelectButton() { return upButton() || downButton(); }

/*
  --- --- ARDUINO METHODS --- ---
*/
void setup() 
{
  // set buzzer to output and disable it
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // set buttons to input
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(12, INPUT);

  // start the dht sensor
  dht.begin();

  // start the display
  mx.begin();
  setBrightness(brightness);
  
  createAccessPoint();
  displayScrollText(createAPText, scrollTextSpeed);
  
  server.on("/", handleRoot);
  
  server.on("/temp", sendTemperature);
  server.on("/humi", sendHumidity);
  server.on("/stat", sendState);
  server.on("/spee", sendSpeed);
  server.on("/brig", sendBrightness);
  server.on("/text", sendText);
  server.on("/cust", sendCustom);
  
  server.begin();

  textIndex = 1;
  activateDisplay();
}
void loop() 
{
  server.handleClient();

  if (!displayActive)
  {
    if (modeButton())
    {
      while(modeButton())
        delay(5);
      activateDisplay();
    }
    return;
  }

  if (millis() - previousRefreshTime >= scrollTextSpeed)
  {
    updateScrollText();
    previousRefreshTime = millis();
  }

  checkButtons();
}

void createAccessPoint()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  delay(500);

  WiFi.softAP(ssid, password);

  ipText = WiFi.softAPIP().toString();
}

void activateDisplay()
{
  displayActive = true;
  updateDisplay();
}
void deactivateDisplay()
{
  displayActive = false;
  mx.clear();
}

void checkButtons()
{
  if (modeButton())
  {
    while (modeButton())
      delay(5);
    deactivateDisplay();
  }
  else if (upButton())
  {
    while (upButton()) 
      delay(5);
    nextMode();
  }
  else if (downButton())
  {
    while (downButton())
      delay(5);
    previousMode();
  }
}


void nextMode()
{
  textIndex = (textIndex + 1) % 7;
  updateDisplay();
}
void previousMode()
{
  textIndex = (textIndex - 1) < 0 ? 6 : (textIndex - 1);
  updateDisplay();
}
void setMode(int _index)
{
  textIndex = _index;
  updateDisplay();
}
void updateDisplay()
{
  if (textIndex == 0)
    setScrollText(ipText);
  else if (textIndex < 6)
    setScrollText(defaultTexts[textIndex - 1]);
  else if (textIndex == 6)
    setScrollText(customText);
}

void setBrightness(int b)
{
  brightness = constrain(b, 0, 15);
  mx.control(MD_MAX72XX::INTENSITY, brightness);
}



/*
  --- --- --- --- --- --- WebServer --- --- --- --- --- ---
*/
void handleRoot()
{
  server.send(200, "text/html", index_html);
  for (int i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "SPEED")
    {
      scrollTextSpeed = server.arg(i).toInt();
    }
    else if (server.argName(i) == "BRIGHTNESS") 
    {
      setBrightness(server.arg(i).toInt());
    }
    else if (server.argName(i) == "CUSTOMTEXT")
    {
      customText = server.arg(i);
      setMode(6);
      activateDisplay();
    }
    else if (server.argName(i) == "TOGGLE")
    {
      if (!displayActive)
        activateDisplay();
      else
        deactivateDisplay();
    }
    else if (server.argName(i) == "SELECTION")
    {
      setMode(server.arg(i).toInt());
    }
  }
}
void sendTemperature() 
{
  // dht.readTemperature(true) will output °F
  server.send(200, "text/plane", "Temperature: " + String(dht.readTemperature(), 1) + "°C");
}
void sendHumidity() 
{
  server.send(200, "text/plane", "Humidity: " + String(dht.readHumidity(), 1) + "%");
}
void sendState() 
{
  String s = displayActive ? "on" : "off";
  server.send(200, "text/plane", "Display: " + s);
}
void sendSpeed() 
{
  server.send(200, "text/plane", String(scrollTextSpeed));
}
void sendBrightness() 
{
  server.send(200, "text/plane", String(brightness));
}
void sendText() 
{
  server.send(200, "text/plane", "Current text: " + currentDisplayText);
}
void sendCustom() 
{
  server.send(200, "text/plane", customText);
}


/*
  --- --- --- --- --- --- DISPLAY --- --- --- --- --- ---
*/
void displayScrollText(String text, int textDelay)
{
  int textLen = text.length();

  byte charWidth;
  byte cBuf[8];

  mx.clear();

  for (int i = 0; i < textLen + 32; i++)
  {
    if (i < textLen)
      charWidth = mx.getChar(text[i], sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
    else
      charWidth = 0;

    for (byte f = 0; f <= charWidth; f++)
    {
      mx.transform(MD_MAX72XX::TSL);
      if (f < charWidth)
        mx.setColumn(0, cBuf[f]);
      delay(textDelay);
    }
  }
}

void resetScrollText()
{
  currentTextIndex = 0;
  currentCharIndex = 0;
  currentCharLength = 0;
}
void setScrollText(String t)
{
  resetScrollText();
  mx.clear();
  currentDisplayText = t;
}
void updateScrollText()
{
  mx.transform(MD_MAX72XX::TSL);
  bool hasChar = currentCharIndex < currentCharLength;

  if (hasChar)
  {
    mx.setColumn(0, currentCharBuffer[currentCharIndex]);
    currentCharIndex++;
  }
  else if (currentTextIndex < currentDisplayText.length())
  {
    currentCharIndex = 0;
    currentCharLength = mx.getChar(currentDisplayText[currentTextIndex], sizeof(currentCharBuffer) / sizeof(currentCharBuffer[0]), currentCharBuffer);
    currentTextIndex++;
  }
  else 
  {
    currentTextIndex++;
    if (currentTextIndex >= currentDisplayText.length() + 32)
      resetScrollText();
  }
}