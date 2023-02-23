#include "preferences.h"

/*
  --- --- --- --- --- --- CLOCK VARIABLES --- --- --- --- --- ---
*/
unsigned long currentTime = millis();
unsigned long previousRefreshTime;
int refreshTime = 500;

bool isSleepmode = true;
unsigned long previousInteraction;

unsigned long lastTimeUpdate;
unsigned long lastWeatherUpdate;

unsigned long startPress;
unsigned long startedTimer;

int modeIndex = 0; // 0=time, 1=weather, 2=timer, 3=settings
bool isSettingsMode = false;
int settingsModeIndex = 0; // 0=return, 1=time, 2=alarm, 3=brightness, 4=timeout, 5=wifi

int timeoutIndex = 2;
int timeout = 0;

bool twelveHourMode = false;

int brightness = 4;

bool alarmEnabled = false;
bool alarmStopped = false;
int alarmHour = 8;
int alarmMinute = 0;

bool timerActive = false;
int timerHour = 0;
int timerMinute = 15;
int maxTimerValue;


/*
  --- --- --- --- --- --- METHODS --- --- --- --- --- ---
*/

/* --- Alarm --- */
bool isAlarm() { return clkTime.isHour(alarmHour) && clkTime.isMinute(alarmMinute); }

/* --- Timer --- */
int getMaxTimerValue() { return timerHour * 3600 + timerMinute * 60; }

/* --- BUZZER --- */
void setBuzzer(bool value) { digitalWrite(BUZZER, value); }
void setBuzzer(int time) { setBuzzer(true); delay(time); setBuzzer(false); }


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


  // start the display
  mx.begin();

  // set the timeout of the display
  timeout = timeoutTimes[timeoutIndex];

  // set the settings of the display
  setBrightness(brightness);


  EEPROM.begin(512);
  delay(10);
  readEEPROM();

  connectToWiFi(true);

  delay(500);

  if (isConnected())
  {
    timeClient.update();
    setTimeFromNTP();

    weatherResultText = getWeatherAPIResult();
  }

  deactivateSleepmode();
}
void loop() 
{
  // is the clock in sleepmode?
  if (isSleepmode)
  {
    currentTime = millis();
    if (currentTime - previousRefreshTime >= 2000)
    {
      checkAlarm();
      checkTimer();
      updateTime();
      previousRefreshTime = millis();
    }

    // return to displaymode when pressed a button or exceededVolume
    if (anyButton())
    {
      // wait for the user to release the button
      while (anyButton()) { delay(5); }
      // deactivate the sleepmode
      deactivateSleepmode();
    }
    return;
  }

  // is the mode button pressed
  if (modeButton())
  {
    bool shortPress = modeButtonShort();
    
    if (!isSettingsMode)
    {
      if (modeIndex == 0)
        timeModeButton(0, shortPress);
      else if (modeIndex == 1)
        weatherModeButton(0, shortPress);
      else if (modeIndex == 2)
        timerModeButton(0, shortPress);
      else if (modeIndex == 3)
        settingsModeButton(0, shortPress);
    }
    else
    {
      if (settingsModeIndex == 0)
        sReturnModeButton(0, shortPress);
      else if (settingsModeIndex == 1)
        sTimeModeButton(0, shortPress);
      else if (settingsModeIndex == 2)
        sAlarmModeButton(0, shortPress);
      else if (settingsModeIndex == 3)
        sBrightnessModeButton(0, shortPress);
      else if (settingsModeIndex == 4)
        sTimeoutModeButton(0, shortPress);
      else if (settingsModeIndex == 5)
        sWifiModeButton(0, shortPress);
    }
  }
  // is the up button pressed
  else if (upButton())
  {
    bool shortPress = upButtonShort();
    
    if (!isSettingsMode)
    {
      if (modeIndex == 0)
        timeModeButton(1, shortPress);
      else if (modeIndex == 1)
        weatherModeButton(1, shortPress);
      else if (modeIndex == 2)
        timerModeButton(1, shortPress);
      else if (modeIndex == 3)
        settingsModeButton(1, shortPress);
    }
    else
    {
      if (settingsModeIndex == 0)
        sReturnModeButton(1, shortPress);
      else if (settingsModeIndex == 1)
        sTimeModeButton(1, shortPress);
      else if (settingsModeIndex == 2)
        sAlarmModeButton(1, shortPress);
      else if (settingsModeIndex == 3)
        sBrightnessModeButton(1, shortPress);
      else if (settingsModeIndex == 4)
        sTimeoutModeButton(1, shortPress);
      else if (settingsModeIndex == 5)
        sWifiModeButton(1, shortPress);
    }
  }
  // is the down button pressed
  else if (downButton())
  {
    bool shortPress = downButtonShort();
      
    if (!isSettingsMode)
    {
      if (modeIndex == 0)
        timeModeButton(2, shortPress);
      else if (modeIndex == 1)
        weatherModeButton(2, shortPress);
      else if (modeIndex == 2)
        timerModeButton(2, shortPress);
      else if (modeIndex == 3)
        settingsModeButton(2, shortPress);
    }
    else
    {
      if (settingsModeIndex == 0)
        sReturnModeButton(2, shortPress);
      else if (settingsModeIndex == 1)
        sTimeModeButton(2, shortPress);
      else if (settingsModeIndex == 2)
        sAlarmModeButton(2, shortPress);
      else if (settingsModeIndex == 3)
        sBrightnessModeButton(2, shortPress);
      else if (settingsModeIndex == 4)
        sTimeoutModeButton(2, shortPress);
      else if (settingsModeIndex == 5)
        sWifiModeButton(2, shortPress);
    }
  }

  currentTime = millis();
  if (currentTime - previousRefreshTime >= refreshTime)
  {
    updateDisplay();

    if (!isSettingsMode)
    {
      checkSleepmode();
      checkAlarm();
      checkTimer();
      updateTime();
    }

    previousRefreshTime = millis();
  }
}

void updateDisplay()
{
  if (isSettingsMode)
  {
    // return
    if (settingsModeIndex == 0)
    {
      displayIconText(returnIcon, returnText, 0);
    }
    // time set
    else if (settingsModeIndex == 1)
    {
      displayIconText(timeIcon, timeText, 0);
    }
    // alarm
    else if (settingsModeIndex == 2)
    {
      displayIconText(alarmIcon, alarmText, 0);
    }
    // brightness
    else if (settingsModeIndex == 3)
    {
      displayIcon(brightnessIcon, 0);

      int v = 23 - map(brightness, 0, 15, 0, 23);
      for (int i = 0; i < 24; i++)
        mx.setColumn(i, v <= i ? 24 : 0);
    }
    // timeout
    else if (settingsModeIndex == 4) 
    { 
      displayIconText(timeoutIcon, String(timeout / 1000) + "s", 0);
    }
    // wifi
    else if (settingsModeIndex == 5)
    {
      displayIconText(wifiIcon, wifiText, 0);
    }
  }
  else
  {
    // Time mode
    if (modeIndex == 0)
    {
      String hour = clkTime.getPhrasedHours(twelveHourMode);
      String minute = clkTime.getPhrasedMinutes();

      displayText(hour + ":" + minute, -4);

      if (alarmEnabled)
        mx.setPoint(0, 0, true);
      if (timerActive)
        mx.setPoint(0, 31, true);
      if (isConnected())
        mx.setPoint(7, 0, true);
    }
    // weather mode
    else if (modeIndex == 1)
    {
      displayIconText(weatherIcon, weatherText, 0);
    }
    // timer mode
    else if (modeIndex == 2)
    {
      if (!timerActive)
      {
        displayIconText(timerIcon, timerText, 0);
      }
      else
      {
        int timeleftSec = maxTimerValue - ((millis() - startedTimer) / 1000);
        int h = (timeleftSec / 3600) % 3;
        int m = (timeleftSec / 60) % 60;
        
        if (h > 0 || m > 0)
          displayIconText(timerActiveIcon, String(h) + ":" + (m < 10 ? "0" + String(m) : String(m)), 0);
        else
          displayIconText(timerActiveIcon, "0:" + String(timeleftSec % 60), 0);
      }
    }
    // settings mode
    else if (modeIndex == 3)
    {
      displayIconText(settingsIcon, settingsText, 0);
    }
  }
}
void updateTime()
{
  if (millis() - lastTimeUpdate >= timeUpdateInterval)
  {
    lastTimeUpdate = millis();
    if (isConnected())
    {
      timeClient.forceUpdate();
      setTimeFromNTP();
    }
    else
    {
      WiFi.disconnect();
      WiFi.begin(ssid, password);
    }
  }
}
void setTimeFromNTP()
{
  clkTime.setHour(timeClient.getHours());
  clkTime.setMinute(timeClient.getMinutes());
  clkTime.setSecond(timeClient.getSeconds());
  clkTime.resetMilliseconds();
}

void setLastInteraction()
{
  previousInteraction = millis();
}
void activateSleepmode()
{
  isSettingsMode = false;
  isSleepmode = true;

  modeIndex = 0;
  settingsModeIndex = 0;

  mx.clear();
}
void deactivateSleepmode()
{
  isSleepmode = false;
  modeIndex = 0;

  isSettingsMode = false;
  settingsModeIndex = 0;

  setLastInteraction();
  updateDisplay();
}

void checkSleepmode()
{
  currentTime = millis();
  if (currentTime - previousInteraction >= timeout)
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
  if (!timerActive)
    return;

  if ((millis() - startedTimer) / 1000 >= maxTimerValue)
    showTimer();
}

void setBrightness(int b)
{
  brightness = constrain(b, 0, 15);
  mx.control(MD_MAX72XX::INTENSITY, brightness);
}

void nextMode()
{
  if (!isSettingsMode)
    modeIndex = (modeIndex + 1) % 4;
  else
    settingsModeIndex = (settingsModeIndex + 1) % 6;
}
void previousMode()
{
  if (!isSettingsMode)
    modeIndex = (modeIndex - 1) < 0 ? 3 : (modeIndex - 1);
  else
    settingsModeIndex = (settingsModeIndex - 1) < 0 ? 5 : (settingsModeIndex - 1);
}