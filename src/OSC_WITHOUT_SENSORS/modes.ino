/*
  --- --- MAIN MODE METHODS --- ---
*/
void timeModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
  {
    if (shortPress)
        alarmEnabled = !alarmEnabled;
    else
        twelveHourMode = !twelveHourMode;
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();

  setLastInteraction();
  updateDisplay();
}
void weatherModeButton(int buttonIndex, bool shortPress)
{
    // mode button
  if (buttonIndex == 0)
  {
    if (!isConnected())
    {
      connectToWiFi(false);
      if (!isConnected())
      {
        setLastInteraction();
        return;
      }
    }
    if (millis() - lastWeatherUpdate >= weatherUpdateInterval)
    {
      lastWeatherUpdate = millis();
      weatherResultText = getWeatherAPIResult();
    }
    
    displayScrollText(weatherResultText, scrollTextSpeed);
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();

  setLastInteraction();
  updateDisplay();
}
void timerModeButton(int buttonIndex, bool shortPress)
{
    // mode button
  if (buttonIndex == 0)
  {
    if (!shortPress && !timerActive)
        adjustingTimer();
    else if (shortPress)
    {
      timerActive = !timerActive;
      if (timerActive)
      {
        maxTimerValue = getMaxTimerValue();
        startedTimer = millis();
      }
    }
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();

  setLastInteraction();
  updateDisplay();
}
void settingsModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
  {
    isSettingsMode = true;
    settingsModeIndex = 0;
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();

  setLastInteraction();
  updateDisplay();
}

/*
  --- --- SETTINGS MODE METHODS --- ---
*/
void sReturnModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
  {
    isSettingsMode = false;
    modeIndex = 3;
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();

  setLastInteraction();
  updateDisplay();
}
void sTimeModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
      adjustingTime();
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();

  updateDisplay();
}
void sAlarmModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
    adjustingAlarm();
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();
    
  updateDisplay();
}
void sBrightnessModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
  {
    setBrightness((brightness + 2) % 16);
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();
    
  updateDisplay();
}
void sTimeoutModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
  {
    timeoutIndex = (timeoutIndex + 1) % 6;
    timeout = timeoutTimes[timeoutIndex];
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();

  updateDisplay();
}
void sWifiModeButton(int buttonIndex, bool shortPress)
{
  // mode button
  if (buttonIndex == 0)
  {
    if (!isConnected() && !shortPress)
    {
      delay(500);
      ESP.reset();
    }
  }
  // up button
  else if (buttonIndex == 1)
    nextMode();
  // down button
  else if (buttonIndex == 2)
    previousMode();
    
  updateDisplay();
}

/*
  --- --- OTHER --- ---
*/
void showAlarmMode()
{
  displayIconText(alarmIcon, alarmActiveText, 0);
  bool buz = false;
  int i = 0;
  previousRefreshTime = millis();
  setBuzzer(false);

  while (!alarmStopped)
  {
    if (anyButton())
    {
      setBuzzer(false);
      while (anyButton()) delay(5);
      alarmStopped = true;
      deactivateSleepmode();
      break;
    }

    currentTime = millis();
    if (currentTime - previousRefreshTime >= alarmSoundTime[i])
    {
      i = (i + 1) % alarmSoundLength;
      buz = !buz;
      setBuzzer(buz);

      if (!isAlarm())
      {
        setBuzzer(false);
        deactivateSleepmode();
        break;
      }

      previousRefreshTime = currentTime;
    }

    delay(5);
  }
}
void showTimer()
{
  displayIconText(timerIcon, timerDoneText, 0);
  bool buz = false;
  int i = 0;
  previousRefreshTime = millis();
  setBuzzer(false);

  while (true)
  {
    if (anyButton())
    {
      setBuzzer(false);
      while (anyButton()) delay(5);
      timerActive = false;
      deactivateSleepmode();
      break;
    }

    currentTime = millis();
    if (currentTime - previousRefreshTime >= alarmSoundTime[i])
    {
      i = (i + 1) % alarmSoundLength;
      buz = !buz;
      setBuzzer(buz);

      if ((millis() - startedTimer) / 1000 >= maxTimerValue + 60)
      {
        setBuzzer(false);
        timerActive = false;
        deactivateSleepmode();
        break;
      }

      previousRefreshTime = currentTime;
    }

    delay(5);
  }
}