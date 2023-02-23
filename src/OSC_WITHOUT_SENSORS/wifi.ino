/*
  --- --- --- --- --- --- WiFi CONNECTION --- --- --- --- --- ---
*/
void connectToWiFi(bool createAP)
{
  displayScrollText(wifiConnectionText, scrollTextSpeed);

  WiFi.disconnect();
  timeClient.end();
  client.stop();

  if (!tryWiFiConnection())
  {
    if (createAP)
    {
      displayScrollText(failedConnectionText, scrollTextSpeed);

      setScrollText(createAccessPointText);
      previousInteraction = millis();
      previousRefreshTime = millis();
      while (millis() - previousInteraction < 25000)
      {
        if (anyButton())
        {
          while (anyButton()) delay(5);
          return;
        }
        if (millis() - previousRefreshTime > scrollTextSpeed)
        {
          updateScrollText();
          previousRefreshTime = millis();
        }

        delay(5);
      }

      createAccessPoint();
    }
    else
      displayScrollText(failedConnectionText, scrollTextSpeed);
  }
}
bool tryWiFiConnection() 
{
  WiFi.begin(ssid, password);
  mx.clear();

  for (int i = 0; i < WIFICONNECTIONTIMEOUT; i++)
  {
    if (isConnected())
      return true;
    else if (WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_WRONG_PASSWORD)
      return false;

    delay(250);
    int c = map(i, 0, WIFICONNECTIONTIMEOUT, 0, 31);
    mx.setColumn(i, c <= i ? 24 : 0);
  }
  return false;
}

bool currentConnectionStatus = false;
bool isConnected()
{
  bool status = WiFi.status() == WL_CONNECTED;
  if (currentConnectionStatus != status)
  {
    currentConnectionStatus = status;
    if (status)
    {
      timeClient.begin();
    }
    else
    {
      timeClient.end();
      client.stop();
    }
  }

  return status;
}

/*
  --- --- --- --- --- --- WEATHER API REQUEST --- --- --- --- --- ---
*/
String getWeatherAPIResult()
{
  displayText(loadingText, 0);
  client.stop();

  if (client.connect("api.openweathermap.org", 80))
  {
    client.println("GET /data/2.5/weather?q=" + cityName + "&appid=" + weatherAPIKey + "&mode=json&units=metric&lang=de&cnt=2 HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else 
  {
    displayScrollText(failedConnectionText, scrollTextSpeed);
    return "no data";
  }

  unsigned long timeout = millis();
  while (client.available() == 0) 
  {
    if (millis() - timeout > 5000) 
    {
      client.stop();
      displayScrollText(failedConnectionText, scrollTextSpeed);
      return "no data";
    }
  }

  String text;
  bool jsonStarted = false;

  while (client.available()) 
  {
    char c = client.read();

    if (!jsonStarted && c == '{')
      jsonStarted = true;
    
    if (jsonStarted)
    {
      text += c;
      delay(5);
    }
  }

  client.stop();


  /* See: https://arduinojson.org/v5/assistant/ for more information */
  const size_t capacity = JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(14) + 480;
  DynamicJsonBuffer jsonBuffer(capacity);

  JsonObject& root = jsonBuffer.parseObject(text.c_str());
  if (!root.success())
  {
    displayScrollText(failedConnectionText, scrollTextSpeed);
    return "no data";
  }

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

    String result = "+++ " + weatherDesc + ", " + String(temp, 1) + "'C, feels like " + String(tempLike, 1) + "'C";
    result += " +++ Clouds " + String(clouds) + "%";
    result += " +++ Wind " + String(windSpeed * 3.6, 1) + "km/h  from " + windDirection + " +++";

    return result;
  }

  displayScrollText(failedConnectionText, scrollTextSpeed);
  return "no data";
}


/*
  --- --- --- --- --- --- ACCESS POINT --- --- --- --- --- ---
*/
void createAccessPoint()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  delay(500);

  String n = "OSC" + String(random(0, 9999));
  WiFi.softAP(n, "");

  displayScrollText(apNameText + n, scrollTextSpeed);

  setScrollText(WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/ssid", sendSSID);
  server.on("/password", sendPassword);
  server.on("/apikey", sendAPIKey);
  server.on("/scan", sendScanData);

  server.begin();


  previousRefreshTime = millis();
  while(true)
  {
    server.handleClient();

    if (millis() - previousRefreshTime > scrollTextSpeed)
    {
      updateScrollText();
      previousRefreshTime = millis();
    }

    delay(5);
  }
}
void handleRoot()
{
  if (server.args() == 3)
  {
    server.send(200, "text/plain", "");

    String argSSID = "";
    String argPassword = "";
    String argAPIKey = "";

    for (int i = 0; i < server.args(); i++)
    {
      if (server.argName(i) == "SSID")
        argSSID = server.arg(i);
      else if (server.argName(i) == "PASSWORD") 
        argPassword = server.arg(i);
      else if (server.argName(i) == "APIKEY")
        argAPIKey = server.arg(i);
    }
    
    saveEEPROM(argSSID, argPassword, argAPIKey);
  }

  server.send(200, "text/html", index_html);
}
void sendSSID() { server.send(200, "text/plane", ssid); }
void sendPassword() { server.send(200, "text/plane", password); }
void sendAPIKey() { server.send(200, "text/plane", weatherAPIKey); }
void sendScanData() { server.send(200, "text/plane", scanNetworks()); }
String scanNetworks()
{
  String results;
  int n = WiFi.scanNetworks();
  if (n == 0)
    results = "no networks found";
  else
  {
    for (int i = 0; i < n; ++i)
    {
      results += String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ")";
      results += WiFi.encryptionType(i) == ENC_TYPE_NONE ? "" : "*";
      results += "\n";
    }
  }
  return results;
}