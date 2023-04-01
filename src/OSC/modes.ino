/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

/*
  --- --- MAIN MODE METHODS --- ---
*/
void timeModeButton(bool shortPress)
{
  if (shortPress)
    alarmEnabled = !alarmEnabled;
  else
    twelveHourMode = !twelveHourMode;

  setLastInteraction();
  updateDisplay();
}
void audioVisualizerModeButton(bool shortPress)
{
  if (shortPress)
    visualizer();
  else
    adjustAudioVisualizer();

  modeChanged();
}
void weatherModeButton()
{
  if (millis() - _lastWeatherUpdateTimestamp >= weatherUpdateInterval)
  {
    // try connecting
    if (!isConnected())
    {
      if(!tryWifiConnection())
        displayScrollText(failedText, textScrollTime);
    }
    // if a connection is established, try to download the data
    if (isConnected())
      weatherResultText = getWeatherAPIResult();
    _lastWeatherUpdateTimestamp = millis();
  }
  
  displayScrollText(weatherResultText, textScrollTime);

  modeChanged();
}
void timerModeButton(bool shortPress)
{
  if (!shortPress && !timer.isActive())
    adjustingTimer();
  else if (shortPress)
  {
    if (timer.isActive())
      timer.stop();
    else
      timer.start();
  }

  setLastInteraction();
  updateDisplay();
}
void stopwatchModeButton()
{
  showStopwatch();
  modeChanged();
}
void settingsModeButton()
{
  isInSettingsmode = true;
  clockMode = 7;

  modeChanged();
}

/*
  --- --- SETTINGS MODE METHODS --- ---
*/
void settingsReturnModeButton()
{
  isInSettingsmode = false;
  clockMode = 6;

  modeChanged();
}
void settingsTimeModeButton()
{
  adjustingTime();
  updateDisplay();
}
void settingsAlarmModeButton()
{
  adjustingAlarm();
  updateDisplay();
}
void settingsBrightnessDayModeButton()
{
  setBrightness((brightnessDay + 2) % 16, false);
  mx.control(MD_MAX72XX::INTENSITY, brightnessDay);
  updateDisplay();
}
void settingsBrightnessNightModeButton()
{
  setBrightness((brightnessNight + 2) % 16, true);
  mx.control(MD_MAX72XX::INTENSITY, brightnessNight);
  updateDisplay();
}
void settingsDayTimeModeButton()
{
  adjustingDayTime();
  modeChanged();
}
void settingsMicrophoneSensivityModeButton()
{
  adjustingMicrophoneSensivity();
  modeChanged();
}
void settingsTimeoutModeButton()
{
  timeoutIndex = (timeoutIndex + 1) % 6;
  timeout = timeoutTimes[timeoutIndex];

  updateDisplay();
}
void settingsTextSpeedModeButton()
{
  textScrollTime = (textScrollTime + 10) > 50 ? 10 : (textScrollTime + 10);
}
void settingsWifiInfoModeButton()
{
  String t = wiFiInfoText1 + ssid;
  t += wiFiInfoText2 + WiFi.localIP().toString();
  t += wifiInfoText3 + WiFi.RSSI() + "dBm";
  displayScrollText(t, textScrollTime);
  modeChanged();
}
void settingsWiFiDataSetModeButton(bool shortPress)
{
  if (!shortPress)
    accessPointConfirmation(reenterDataText);
  modeChanged();
}


/*
  --- --- ADJUST METHODS --- ---
*/
void adjustingTime()
{
  int hours = clkTime.getHours();
  int minutes = clkTime.getMinutes();
  int seconds = clkTime.getSeconds();

  int hoursBefore = hours;
  int minutesBefore = minutes;
  int secondsBefore = seconds;

  int selection = 0;

  updateAdjustTimeDisplay(hours, minutes, selection);

  while (true)
  {
    // Check input
    int b = -1;
    if (modeButton())
    {
      bool shortPress = modeButtonShort(true);
      if (!shortPress)
        break;
      b = 0;
    }
    else if (upButton())
    {
      while (upButton()) 
        delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) 
        delay(5);
      b = 2;
    }


    // switch modes
    if (b == 0)
    {
      selection = (selection + 1) % 4;
      if (selection == 2)
        updateAdjustTimeDisplay(minutes, seconds, selection);
      else
        updateAdjustTimeDisplay(hours, minutes, selection);
    }
    else if (b == 1 || b == 2)
    {
      // hour mode & up or down
      if (selection == 0)
      { 
        hours = (b == 1) ? hours + 1 : hours - 1;
        if (hours < 0)
          hours = 23;
        else if (hours > 23)
          hours = 0;

        updateAdjustTimeDisplay(hours, minutes, selection);
      }
      // minutes mode & up or down
      else if (selection == 1)
      {
        minutes = (b == 1) ? minutes + 1 : minutes - 1;
        if (minutes < 0)
          minutes = 59;
        else if (minutes > 59)
          minutes = 0;

        updateAdjustTimeDisplay(hours, minutes, selection);
      }
      // seconds mode & up or down
      else if (selection == 2)
      {
        seconds = (b == 1) ? seconds + 1 : seconds - 1;
        if (seconds < 0)
          seconds = 59;
        else if (seconds > 59)
          seconds = 0;

        updateAdjustTimeDisplay(minutes, seconds, selection);
      }
      else if (selection == 3)
      {
        summerTime = !summerTime;
        updateAdjustTimeDisplay(minutes, seconds, selection);
      }
    }

    // wait for 5 milliseconds
    delay(5);
  }

  if (hoursBefore != hours)
    clkTime.setHour(hours);
  if (minutesBefore != minutes)
    clkTime.setMinute(minutes);
  if (secondsBefore != seconds) 
  {
    clkTime.setSecond(seconds);
    clkTime.resetMilliseconds();
  }
}
void updateAdjustTimeDisplay(int first, int second, int selection)
{
  if (selection != 3)
    showSetTime(first, second, selection == 0, ':', -3);
  else
    displayText(summerTime ? "+1h" : "+0h", 0);
}

void adjustingAlarm()
{
  bool hSel = true;
  updateAdjustAlarmDisplay(hSel);

  while (true)
  {
    int b = -1;
    if (modeButton())
    {
      bool shortPress = modeButtonShort(true);

      if (!shortPress)
        break;
      b = 0;
    }
    else if (upButton())
    {
      while (upButton()) 
        delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) 
        delay(5);
      b = 2;
    }

    // switch mode
    if (b == 0)
    {
      hSel = !hSel;
      updateAdjustAlarmDisplay(hSel);
    }
    else if (b == 1 || b == 2)
    {
      // hours
      if (hSel) 
      {
        // up
        if (b == 1)
          alarmHour = (alarmHour + 1) % 24;
        // down
        else if (b == 2)
          alarmHour = (alarmHour - 1) < 0 ? 23 : (alarmHour - 1);
        updateAdjustAlarmDisplay(hSel);
      }
      // minutes
      else
      {
        // up
        if (b == 1)
          alarmMinute = (alarmMinute + 1) % 60;
        // down
        else if (b == 2)
          alarmMinute = (alarmMinute - 1) < 0 ? 59 : (alarmMinute - 1);
        updateAdjustAlarmDisplay(hSel);
      }
    }

    // wait for 5 milliseconds
    delay(5);
  }
}
void updateAdjustAlarmDisplay(bool hours) 
{
  showSetTime(alarmHour, alarmMinute, hours, ':', -3);
}

void adjustingTimer()
{
  int hours = timer.getHours();
  int minutes = timer.getMinutes();

  bool hSel = true;
  updateAdjustTimerDisplay(hours, minutes, hSel);

  while (true)
  {
    int b = -1;
    if (modeButton())
    {
      bool shortPress = modeButtonShort(true);

      if (!shortPress)
        break;
      b = 0;
    }
    else if (upButton())
    {
      while (upButton()) 
        delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) 
        delay(5);
      b = 2;
    }

    // switch mode
    if (b == 0)
    {
      hSel = !hSel;
      updateAdjustTimerDisplay(hours, minutes, hSel);
    }
    else if (b == 1 || b == 2)
    {
      // hours
      if (hSel) 
      {
        // up
        if (b == 1)
          hours = (hours + 1) % 5;
        // down
        else if (b == 2)
          hours = (hours - 1) < 0 ? 4 : (hours - 1);
        updateAdjustTimerDisplay(hours, minutes, hSel);
      }
      // minutes
      else
      {
        // up
        if (b == 1)
          minutes = (minutes + 1) % 60;
        // down
        else if (b == 2)
          minutes = (minutes - 1) < 0 ? 59 : (minutes - 1);
        updateAdjustTimerDisplay(hours, minutes, hSel);
      }
    }
    

    // wait for 5 milliseconds
    delay(5);
  }

  if (hours == 0 && minutes == 0)
    minutes = 1;
  timer.setHours(hours);
  timer.setMinutes(minutes);
}
void updateAdjustTimerDisplay(int hours, int minutes, bool hour) 
{
  showSetTime(hours, minutes, hour, ':', -4);
}

void adjustingDayTime()
{
  bool startSel = true;
  updateAdjustDayTimeDisplay(startSel);

  while (true)
  {
    int b = -1;
    if (modeButton())
    {
      bool shortPress = modeButtonShort(true);

      if (!shortPress)
        break;
      b = 0;
    }
    else if (upButton())
    {
      while (upButton()) 
        delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) 
        delay(5);
      b = 2;
    }

    // switch mode
    if (b == 0)
    {
      startSel = !startSel;
      updateAdjustDayTimeDisplay(startSel);
    }
    else if (b == 1 || b == 2)
    {
      // hours
      if (startSel) 
      {
        // up
        if (b == 1)
          nightStart = (nightStart + 1) % 24;
        // down
        else if (b == 2)
          nightStart = (nightStart - 1) < 0 ? 23 : (nightStart - 1);
        updateAdjustDayTimeDisplay(startSel);
      }
      // minutes
      else
      {
        // up
        if (b == 1)
          nightEnd = (nightEnd + 1) % 24;
        // down
        else if (b == 2)
          nightEnd = (nightEnd - 1) < 0 ? 23 : (nightEnd - 1);
        updateAdjustDayTimeDisplay(startSel);
      }
    }

    // wait for 5 milliseconds
    delay(5);
  }
}
void updateAdjustDayTimeDisplay(bool selection)
{
  showSetTime(nightStart, nightEnd, selection, '-', -3);
  checkBrightness(true);
}

void adjustingMicrophoneSensivity()
{
  lastRefreshTimestamp = millis();
    
  while (true)
  {
    // Check input
    int b = 0;
    if (modeButton())
    {
      while (modeButton()) 
        delay(5);
      break;
    }
    else if (upButton())
    {
      while (upButton()) 
        delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) 
        delay(5);
      b = 2;
    }


    // up
    if (b == 1)
      microphoneSensivity = (microphoneSensivity + 1) > 55 ? 35 : (microphoneSensivity + 1);
    // down
    else if (b == 2)
      microphoneSensivity = (microphoneSensivity - 1) < 35 ? 55 : (microphoneSensivity - 1);

    updateMicrophoneSensivity();

    // wait for 5 milliseconds
    delay(5);
  }
}
void updateMicrophoneSensivity()
{
  int v = (int)map(getVolume(), 0, 55, 0, 31);
  int b = (int)map(microphoneSensivity, 0, 55, 31, 0);
  for (int i = 31; i >= 0; i--)
  {
    if (i == b)
      mx.setColumn(i, 31 - i <= v ? 231 : 255);
    else
      mx.setColumn(i, 31 - i <= v ? 24 : 0);
  }
}

/*
  --- --- OTHER --- ---
*/

void showStopwatch()
{
  int h = 0;
  int m = 0;
  int s = 0;
  int ms = 0;
  stpWatch.start();
  lastRefreshTimestamp = millis();

  while (true)
  {
    // wait for user input
    if (anyButton())
    {
      while (anyButton()) 
        delay(5);
      break;
    }

    h = stpWatch.getHours();
    m = stpWatch.getMinutes();


    if (h >= 5)
      break;

    if (h < 1)
    {
      // show seconds:milliseconds
      if (m < 1)
      {
        s = stpWatch.getSeconds();
        ms = stpWatch.getMilliseconds() / 10;
        displayText((s < 10 ? "0" + String(s) : String(s)) + ":" + (ms < 10 ? "0" + String(ms) : String(ms)), -3);
      }
      // show minutes:seconds
      else if (millis() - lastRefreshTimestamp >= 500)
      {
        s = stpWatch.getSeconds();
        displayText((m < 10 ? "0" + String(m) : String(m)) + ":" + (s < 10 ? "0" + String(s) : String(s)), -3);
        lastRefreshTimestamp = millis();
      }
    }
    // show hour:minute
    else if (millis() - lastRefreshTimestamp >= 1000)
      displayText("0" + String(h) + ":" + (m < 10 ? "0" + String(m) : String(m)), -3);

    
    delay(5);
  }

  stpWatch.stop();
  lastRefreshTimestamp = millis();
  setLastInteraction();
  checkBrightness(true);
}
void showAlarmMode()
{
  setIconText(alarmIcon, alarmActiveText);
  bool buz = false;
  int i = 0;
  lastRefreshTimestamp = millis();
  lastInteractionTimestamp = millis();
  setBuzzer(false);

  while (true)
  {
    // wait for user input
    if (anyButton())
    {
      setBuzzer(false);
      while (anyButton()) 
        delay(5);
      alarmStopped = true;
      break;
    }

    // play the sound at the given interval
    if (millis() - lastRefreshTimestamp >= alarmSoundTime[i])
    {
      // stop the alarm if a minute has passed
      if (millis() - lastInteractionTimestamp >= 60000)
        break;
      i = (i + 1) % alarmSoundLength;
      buz = !buz;
      setBuzzer(buz);
      lastRefreshTimestamp = millis();
    }
    // shift the text by one pixel in the given interval
    if (millis() - lastShiftTimestamp >= textScrollTime)
    {
      shiftIconText();
      lastShiftTimestamp = millis();
    }
    
    // wait for 5 milliseconds
    delay(5);
  }

  setBuzzer(false);
  if (isInSleepmode)
    deactivateSleepmode();
  else
  {
    lastRefreshTimestamp = millis();
    setLastInteraction();
    checkBrightness(true);
    modeChanged();
  }
}
void showTimer()
{
  displayIconText(timerIcon, timerDoneText, 0);
  bool buz = false;
  int i = 0;
  lastRefreshTimestamp = millis();
  lastInteractionTimestamp = millis();
  setBuzzer(false);

  while (true)
  {
    // wait for user input
    if (anyButton())
    {
      setBuzzer(false);
      while (anyButton()) 
        delay(5);
      break;
    }

    // play the sound at the given interval
    if (millis() - lastRefreshTimestamp >= alarmSoundTime[i])
    {
      // stop the alarm if a minute has passed
      if (millis() - lastInteractionTimestamp >= 60000)
        break;
      i = (i + 1) % alarmSoundLength;
      buz = !buz;
      setBuzzer(buz);

      lastRefreshTimestamp = millis();
    }

    delay(5);
  }

  timer.stop();
  setBuzzer(false);
  if (isInSleepmode)
    deactivateSleepmode();
  else
  {
    lastRefreshTimestamp = millis();
    setLastInteraction();
    checkBrightness(true);
    modeChanged();
  }
}