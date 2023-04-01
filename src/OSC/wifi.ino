/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

bool _currentConnectionStatus = false;
ESP8266WebServer _apServer = ESP8266WebServer(80);

bool tryWifiConnection() 
{
  // stop the current connection and all clients
  WiFi.disconnect();
  timeClient.end();
  weatherClient.stop();

  // display the connection text
  displayScrollText(tryConnectionText, textScrollTime);

  // connect to the wifi
  WiFi.begin(ssid, password);
  
  // wait for the timeout
  for (int i = 0; i < WIFICONNECTIONTIMEOUT; i++)
  {
    // check the connection status
    if (isConnected())
      return true;
    else if (WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_WRONG_PASSWORD || WiFi.status() == WL_NO_SSID_AVAIL)
      return false;

    // wait for 250 milliseconds and show the loading bar
    delay(250);
    int c = map(i, 0, WIFICONNECTIONTIMEOUT, 0, 31);
    mx.setColumn(i, c <= i ? 24 : 0);
  }

  // return false
  return false;
}
bool isConnected()
{
  // get the current connection status
  bool status = WiFi.status() == WL_CONNECTED;
  // check it with the last connection status
  if (_currentConnectionStatus != status)
  {
    // set the new connection status
    _currentConnectionStatus = status;
    // if it is connected, start the time client
    if (status)
    {
      timeClient.begin();
      // last update was more than or equal to 30 minutes ago
      if (millis() - _lastTimeUpdateTimestamp >= 1800000)
      {
        timeClient.forceUpdate();
        delay(100);
        setTimeFromNTP();
        
        _lastTimeUpdateTimestamp = millis();
      }
    }
    // if it is not connected, stop the clients
    else
    {
      timeClient.end();
      weatherClient.stop();
    }
  }
  // return the status
  return status;
}


void accessPointConfirmation(String text)
{
  // set the scroll text
  setScrollText(text);

  // set the timestamps
  lastInteractionTimestamp = millis();
  lastRefreshTimestamp = millis();

  // check if the timeout has been reached
  while (millis() - lastInteractionTimestamp < ACCESPOINTCONFIRMATIONTIMEOUT)
  {
    // check if the access point should be canceled
    if (anyButton())
    {
      while (anyButton()) 
        delay(5);
      return;
    }

    // shift the text by one pixel
    if (millis() - lastRefreshTimestamp > textScrollTime)
    {
      shiftScrollText();
      lastRefreshTimestamp = millis();
    }

    // wait for 5 milliseconds
    delay(5);
  }

  // create the access point
  createAccessPoint();  
}
void createAccessPoint()
{
  // disconnect
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  // wait half a second
  delay(500);

  // start the access point with no password
  WiFi.softAP("OSC-AP", "");

  // set up and start the server
  _apServer.on("/", handleRoot);
  _apServer.on("/ssid", sendSSID);
  _apServer.on("/password", sendPassword);
  _apServer.on("/apikey", sendAPIKey);
  _apServer.on("/scan", sendScanData);
  _apServer.begin();

  // set the text as 
  setScrollText(accessPointExplanation + WiFi.softAPIP().toString());
  lastRefreshTimestamp = millis();
  while(true)
  {
    // handle the web server
    _apServer.handleClient();

    // shift the text by one pixel
    if (millis() - lastRefreshTimestamp > textScrollTime)
    {
      shiftScrollText();
      lastRefreshTimestamp = millis();
    }

    // wait 5 milliseconds
    delay(5);
  }
}

void handleRoot()
{
  // check if the user pressed the submit button
  if (_apServer.args() == 3)
  {
    _apServer.send(200, "text/plain", dataSavedText);

    // read the values 
    String argSSID = "";
    String argPassword = "";
    String argAPIKey = "";
    for (int i = 0; i < _apServer.args(); i++)
    {
      if (_apServer.argName(i) == "SSID")
        argSSID = _apServer.arg(i);
      else if (_apServer.argName(i) == "PASSWORD") 
        argPassword = _apServer.arg(i);
      else if (_apServer.argName(i) == "APIKEY")
        argAPIKey = _apServer.arg(i);
    }
    
    // save the values to the eeprom and restart
    saveEEPROM(argSSID, argPassword, argAPIKey);
    delay(500);
    ESP.reset();
  }

  _apServer.send(200, "text/html", index_html);
}
void sendSSID() { _apServer.send(200, "text/plane", ssid); }
void sendPassword() { _apServer.send(200, "text/plane", password); }
void sendAPIKey() { _apServer.send(200, "text/plane", weatherAPIKey); }
void sendScanData() { _apServer.send(200, "text/plane", scanNetworks()); }
String scanNetworks()
{
  // get the network length
  int n = WiFi.scanNetworks();
  if (n == 0)
    return noNetworksFoundText;
  // get the results and return them
  String results;
  for (int i = 0; i < n; ++i)
  {
    results += String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ")";
    results += WiFi.encryptionType(i) == ENC_TYPE_NONE ? "" : "*";
    results += "\n";
  }
  return results;
}


String getWeatherAPIResult()
{
  // set the loading text
  displayText(loadingText, 0);

  // stop the weather client
  weatherClient.stop();

  // connect to the openweathermap api and request data
  if (weatherClient.connect("api.openweathermap.org", 80))
  {
    weatherClient.println("GET /data/2.5/weather?q=" + cityName + "&appid=" + weatherAPIKey + "&mode=json&units=metric&lang=de&cnt=2 HTTP/1.1");
    weatherClient.println("Host: api.openweathermap.org");
    weatherClient.println("User-Agent: ArduinoWiFi/1.1");
    weatherClient.println("Connection: close");
    weatherClient.println();
  } 
  else
    return noWeatherDataText;

  // wait for a responce
  unsigned long timeout = millis();
  while (weatherClient.available() == 0) 
  {
    // timeout handling
    if (millis() - timeout > WEATHERAPIREQUESTTIMEOUT) 
    {
      weatherClient.stop();
      return noWeatherDataText;
    }
  }


  String text;
  bool jsonStarted = false;

  // save the responce to the text-variable
  while (weatherClient.available()) 
  {
    // read the char of the responce
    char c = weatherClient.read();

    // check if the json has not been started
    if (!jsonStarted && c == '{')
      jsonStarted = true;
    if (jsonStarted)
    {
      // add the character to the text
      text += c;
      // wait 2 milliseconds
      delay(2);
    }
  }

  // stop the
  weatherClient.stop();


  /* See: https://arduinojson.org/v5/assistant/ for more information */
  const size_t capacity = JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(14) + 480;
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& root = jsonBuffer.parseObject(text.c_str());
  if (!root.success())
    return noWeatherDataText;

  // check if the responce was successful and phrase the data and return it
  if(root["cod"] == 200)
  {
    String weatherDesc = root["weather"][0]["description"];

    JsonObject& main = root["main"];
    float temp = main["temp"];
    float tempLike = main["feels_like"];
    float windSpeed = root["wind"]["speed"];
    int windDeg = root["wind"]["deg"];

    int clouds = root["clouds"]["all"];


    String directions = "N NOO SOS SWW NW";  // wind direction (N NO O SO S SW W NW) always 2 chars long
    int wr = (windDeg + 22) % 360 / 45;
    String windDirection = directions.substring(2 * wr, 2 * wr + 2);
    windDirection.trim();

    String result = "+++ " + weatherDesc;
    result += ", " + String(temp, 1) + "°C";
    result += " +++ wind " + String(windSpeed * 3.6, 1) + "km/h  from " + windDirection + " +++";

    return result;
  }

  return noWeatherDataText;
}