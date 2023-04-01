/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

#include "preferences.h"


/*
  --- --- --- VARIABLES --- --- ---
*/
bool isInSleepmode = true;

// main: 0=time, 1=temperature, 2=audio visualizer, 3=weather, 4=timer, 5=stopwatch, 6=settings 
// settings: 7=back, 8=setTime, 9=setAlarm, 10=brightnessDay, 11=brightnessNight, 12=dayTime, 13=microphonesensivity, 14=timeout, 15=text speed, 16=wifi info, 17=enter wifi data
byte clockMode = 0;
bool isInSettingsmode = false;

unsigned long _lastTimeUpdateTimestamp;
unsigned long _lastWeatherUpdateTimestamp;

bool summerTime = false;
bool twelveHourMode = false;

bool alarmEnabled = false;
bool alarmStopped = false;
int alarmHour = 8;
int alarmMinute = 0;


/*
  --- --- --- METHODS --- --- ---
*/
/* --- ALARM --- */
bool isAlarm() { return (((clkTime.getHours() + (summerTime ? 1 : 0)) % 24) == alarmHour) && clkTime.isMinute(alarmMinute); }

/* --- BUZZER --- */
void setBuzzer(bool value) { digitalWrite(BUZZER, value); }
void setBuzzer(int time) { setBuzzer(true); delay(time); setBuzzer(false); }

/* --- MICROPHONE --- */
int peakToPeak() 
{
  int min = 1024;
  int max = 0;
  for (int i = 0; i < 256; i++)
  {
    int sample = analogRead(MIC);
    if (sample < 1024)
    {
        if (sample > max)
          max = sample;
        else if (sample < min)
          min = sample;
    }
  }
  return max - min;
}
double getVoltage() { return (peakToPeak()  * 3.3) / 1024; }
double getVolume() { return log10(getVoltage() / 0.00631) * 20; }
bool exceededVolume() { return getVolume() >= microphoneSensivity; }



void setup() 
{
  // set buzzer to output and disable it
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // set buttons to input
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(12, INPUT);
  // set microphone to input
  pinMode(MIC, INPUT);


  // start the display
  mx.begin();
  // start the temperatur sensor
  dht.begin();


  timeout = timeoutTimes[timeoutIndex];
  setBrightness(2, false);
  setBrightness(0, true);


  // load the values from the eeprom
  EEPROM.begin(512);
  delay(20);
  readEEPROM();

  // try to connect to the wifi, if successful, get the time and weather data
  if (tryWifiConnection())
  {
    timeClient.forceUpdate();
    delay(100);
    setTimeFromNTP();

    weatherResultText = getWeatherAPIResult();
  }
  else
  {
    displayScrollText(failedText, textScrollTime);
    // ask the user if the access point should be started
    accessPointConfirmation(createAccessPointText);
  }

  deactivateSleepmode();
}
void loop() 
{
  // is the clock in sleepmode?
  if (isInSleepmode)
  {
    // check the alarm, timer and time-update every x time interval
    if (millis() - lastRefreshTimestamp >= SLEEPMODECHECKINTERVAL)
    {
      checkAlarm();
      checkTimer();
      if (isConnected())
        checkTimeUpdate();
      lastRefreshTimestamp = millis();
    }

    // return to displaymode when pressed a button or exceededVolume
    if (anyButton() || exceededVolume())
    {
      // wait for the user to release the button
      while (anyButton())
        delay(5);

      // deactivate the sleepmode
      deactivateSleepmode();
    }
    return;
  }


  if (modeButton())
  {
    bool shortPress = modeButtonShort(makeSoundMode(clockMode));
    switch(clockMode)
    {
      case 0:
        timeModeButton(shortPress);
        break;
      case 2:
        audioVisualizerModeButton(shortPress);
        break;
      case 3:
        weatherModeButton();
        break;
      case 4:
        timerModeButton(shortPress);
        break;
      case 5:
        stopwatchModeButton();
        break;
      case 6:
        settingsModeButton();
        break;
      case 7:
        settingsReturnModeButton();
        break;
      case 8:
        settingsTimeModeButton();
        break;
      case 9:
        settingsAlarmModeButton();
        break;
      case 10:
        settingsBrightnessDayModeButton();
        break;
      case 11:
        settingsBrightnessNightModeButton();
        break;
      case 12:
        settingsDayTimeModeButton();
        break;
      case 13:
        settingsMicrophoneSensivityModeButton();
        break;
      case 14:
        settingsTimeoutModeButton();
        break;
      case 15:
        settingsTextSpeedModeButton();
        break;
      case 16:
        settingsWifiInfoModeButton();
        break;
      case 17:
        settingsWiFiDataSetModeButton(shortPress);
        break;
    }

    setLastInteraction();
  }
  // go to the next mode when the up button is pressed
  else if (upButton())
  {
    while (upButton())
      delay(5);
    nextMode();
  }
  // go to the previous mode when the down button is pressed
  else if (downButton())
  {
    while (downButton())
      delay(5);
    previousMode();
  }

  // check if the current text should be shifted
  if (isShiftMode(clockMode))
  {
    // shift the text in the interval
    if (millis() - lastShiftTimestamp >= textScrollTime)
    {
      if (clockMode != 15)
        shiftIconText();
      else
        shiftScrollText();
      lastShiftTimestamp = millis();
    }
  }
  
  // refresh the display and check for alarm, timer and time-update
  if (millis() - lastRefreshTimestamp >= refreshTime)
  {
    updateDisplay();

    if (!isInSettingsmode)
    {
      checkSleepmode();
      checkAlarm();
      checkTimer();
      checkTimeUpdate();
    }

    if (clockMode != 10 && clockMode != 11)
      checkBrightness(true);

    lastRefreshTimestamp = millis();
  }
}

void updateDisplay()
{
  // time mode
  if (clockMode == 0)
  {
    if (summerTime)
    {
      int h = (clkTime.getHours() + (summerTime ? 1 : 0)) % 24;
      if (twelveHourMode && h > 12)
        h -= 12;
      displayText((h < 10 ? "0" + String(h) : String(h)) + ":" + clkTime.getPhrasedMinutes(), -4);
    }
    else
      displayText(clkTime.getPhrasedHours(twelveHourMode) + ":" + clkTime.getPhrasedMinutes(), -4);
    

    if (alarmEnabled)
      mx.setPoint(0, 0, true);
    if (timer.isActive())
      mx.setPoint(0, 31, true);
    if (isConnected())
      mx.setPoint(7, 0, true);
  }
  // temperature mode
  else if (clockMode == 1)
  {
    String temperatureText = "";

    float t = dht.readTemperature();
    if (isnan(t))
    {
      temperatureText += "---";
      temperatureText += (char)144;
      temperatureText += "C";
    }
    else
    {
      temperatureText += String(t, 1);
      temperatureText += (char)144;
      temperatureText += "C";
    }

    displayText(temperatureText, -1);
  }
  // timer mode
  else if (clockMode == 4)
  {
    if (!timer.isActive()) 
    {
      displayIconText(timerIcon, timerText, 0);
    }
    else
    {
      int h = timer.getRemainingHours();
      int m = timer.getRemainingMinutes();
      
      if (h > 0 || m > 0)
      {
        displayIconText(timerIcon, String(h) + ":" + (m < 10 ? "0" + String(m) : String(m)), 0);
      }
      else 
      {
        displayIconText(timerIcon, "0:" + String(timer.getRemainingSeconds()), 0);
      }
    }
  }
  // settings time mode
  else if (clockMode == 8)
  {
    displayIconText(timeIcon, timeText, 0);
  }
  // settings alarm mode
  else if (clockMode == 9)
  {
    displayIconText(alarmIcon, alarmText, 0);
  }
  // settings brightness day
  else if (clockMode == 10)
  {
    displayIcon(brightnessDayIcon, 0);
    int v = 23 - map(brightnessDay, 0, 15, 0, 23);
    for (int i = 0; i < 24; i++)
      mx.setColumn(i, v <= i ? 24 : 0);
  }
  // settings brightness night
  else if (clockMode == 11)
  {
    displayIcon(brightnessNightIcon, 0);
    int v = 23 - map(brightnessNight, 0, 15, 0, 23);
    for (int i = 0; i < 24; i++)
      mx.setColumn(i, v <= i ? 24 : 0);
  }
  // settings timeout
  else if (clockMode == 14)
  {
    displayIconText(timeoutIcon, String(timeout / 1000) + "s", 0);
  }
}



/* --- sleepmode --- */

void setLastInteraction()
{
  lastInteractionTimestamp = millis();
}
void activateSleepmode()
{
  isInSettingsmode = false;
  isInSleepmode = true;

  clockMode = 0;

  mx.clear();
}
void deactivateSleepmode()
{
  lastRefreshTimestamp = millis();
  isInSleepmode = false;
  clockMode = 0;

  checkBrightness(true);
  updateDisplay();
  setLastInteraction();
}



/* --- checks --- */

void checkSleepmode()
{
  if (millis() - lastInteractionTimestamp >= timeout)
  {
    activateSleepmode();
  }
}
void checkAlarm()
{
  if (alarmStopped)
  {
    if (!isAlarm())
      alarmStopped = false;
    return;
  }

  if (!alarmEnabled)
    return;

  if (isAlarm())
    showAlarmMode();
}
void checkTimer()
{
  if (!timer.isActive())
    return;
  if (timer.hasEnded())
    showTimer();
}
void checkTimeUpdate()
{
  if (millis() - _lastTimeUpdateTimestamp >= timeUpdateInterval)
  {
    _lastTimeUpdateTimestamp = millis();
    if (isConnected())
    {
      timeClient.forceUpdate();
      delay(100);
      setTimeFromNTP();
    }
    else
    {
      WiFi.disconnect();
      WiFi.begin(ssid, password);
    }
  }
}
void checkBrightness(bool setIntensity)
{
  int calcBrightness = brightnessDay;
  int h = (clkTime.getHours() + (summerTime ? 1 : 0)) % 24;
  if (h >= nightStart || h < nightEnd)
    calcBrightness = brightnessNight;

  if (calcBrightness != currentBrightness)
  {
    currentBrightness = calcBrightness;
    if (setIntensity)
      mx.control(MD_MAX72XX::INTENSITY, currentBrightness);
  }
}


/* --- modes --- */

void nextMode()
{
  if (clockMode == 11)
    checkBrightness(true);

  if (isInSettingsmode)
  {
    clockMode = (clockMode + 1) > 17 ? 7 : (clockMode + 1);
    if (clockMode == 16 && !isConnected())
      clockMode++;
  }
  else
    clockMode = (clockMode + 1) % 7;

  modeChanged();
}
void previousMode()
{
  if (clockMode == 10)
    checkBrightness(true);

  if (isInSettingsmode)
  {
    clockMode = (clockMode - 1) < 7 ? 17 : (clockMode - 1);
    if (clockMode == 16 && !isConnected())
      clockMode--;
  }
  else
    clockMode = (clockMode - 1) < 0 ? 6 : (clockMode - 1);

  modeChanged();
}

void modeChanged()
{
  if (isShiftMode(clockMode))
  { 
    // audio visualizer mode
    if (clockMode == 2)
      setIconText(audioVisualizerIcon, audioVisualizerText);
    // weather mode
    else if (clockMode == 3)
      setIconText(weatherIcon, weatherText);
    // stopwatch mode
    else if (clockMode == 5)
      setIconText(stopwatchIcon, stopwatchText);
    // settings mode
    else if (clockMode == 6)
      setIconText(settingsIcon, settingsText);
    // settings: back
    else if (clockMode == 7)
      setIconText(returnIcon, returnText);
    // settings: daytime
    else if (clockMode == 12)
      setIconText(dayTimeIcon, dayTimeText);
    // settings: microphone sensivity
    else if (clockMode == 13)
      setIconText(microphoneSensivityIcon, microphoneSensivityText);
    // settings: text speed
    else if (clockMode == 15)
      setScrollText(textSpeedText);
    // settings: wifi info
    else if (clockMode == 16)
      setIconText(wifiInfoIcon, wifiInfoText);
    // settings: enter wifi data
    else if (clockMode == 17)
      setIconText(wifiDataIcon, wifiDataText);
  }
  else if (clockMode == 10 || clockMode == 11)
  {
    currentBrightness = (clockMode == 10) ? brightnessDay : brightnessNight;
    mx.control(MD_MAX72XX::INTENSITY, currentBrightness);
  }

  setLastInteraction();
  updateDisplay();
  lastShiftTimestamp = millis();
}


/* --- other --- */
void setTimeout(int index)
{
  // set the index in the following array interval: 6=length of the array
  timeoutIndex = index % 6;
  // set the timeout
  timeout = timeoutTimes[timeoutIndex];
}
void setBrightness(int b, bool night)
{
  if (night)
    brightnessNight = constrain(b, 0, 15);
  else
    brightnessDay = constrain(b, 0, 15);
}

void setTimeFromNTP()
{
  clkTime.setHour(timeClient.getHours());
  clkTime.setMinute(timeClient.getMinutes());
  clkTime.setSecond(timeClient.getSeconds());
  clkTime.resetMilliseconds();
}

bool makeSoundMode(int mode)
{
  return mode == 0 || mode == 2 || mode == 4 || mode == 17;
}
bool isShiftMode(int mode)
{
  return mode == 2 || mode == 3 || mode == 5 || mode == 6 || mode == 7 || mode == 12 || mode == 13 || mode == 15 || mode == 16 || mode == 17;
}