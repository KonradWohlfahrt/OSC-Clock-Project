void adjustingTime()
{
  int selection = 0;

  while (true)
  {
    // Check input
    int b = -1;
    if (modeButton())
    {
      bool shortPress = modeButtonShort();
      if (!shortPress)
        break;
      b = 0;
    }
    else if (upButton())
    {
      while (upButton()) delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) delay(5);
      b = 2;
    }


    // switch modes
    if (b == 0)
      selection = (selection + 1) % 3;

    // hour mode & up or down
    if (selection == 0 && (b == 1 || b == 2))
    { 
      int h = clkTime.getAdditionHour();
      // up
      if (b == 1)
        h++;
      // down
      else
        h--;
      
      clkTime.setAdditionHour(h);
      updateAdjustTimeDisplay(selection);
    }
    // minutes mode & up or down
    else if (selection == 1 && (b == 1 || b == 2))
    {
      int m = clkTime.getAdditionMinute();
      // up
      if (b == 1)
        m++;
        // down
      else
        m--;
      
      clkTime.setAdditionMinute(m);
      updateAdjustTimeDisplay(selection);
    }
    // seconds mode & up or down
    else if (selection == 2 && (b == 1 || b == 2))
    {
      clkTime.setSecond(0);
      clkTime.resetMilliseconds();
      updateAdjustTimeDisplay(selection);
    }

    // update the display after 250 milliseconds
    currentTime = millis();
    if (currentTime - previousRefreshTime >= 250)
    {
      updateAdjustTimeDisplay(selection);
      previousRefreshTime = millis();
    }

    // wait for 5 milliseconds
    delay(5);
  }
}
void updateAdjustTimeDisplay(int selection)
{
  if (selection == 0)
    displayText("hrs: " + clkTime.getPhrasedHours(false), 0);
  else if (selection == 1)
    displayText("min: " + clkTime.getPhrasedMinutes(), 0);
  else if (selection == 2)
    displayText("sec: " + clkTime.getPhrasedSeconds(), 0);
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
      bool shortPress = modeButtonShort();

      if (!shortPress)
        break;
      b = 0;
    }
    else if (upButton())
    {
      while (upButton()) delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) delay(5);
      b = 2;
    }

    // switch mode
    if (b == 0)
    {
      hSel = !hSel;
      updateAdjustAlarmDisplay(hSel);
    }
      
        
    // hours
    if (hSel) 
    {
      // up
      if (b == 1)
      {
        alarmHour = (alarmHour + 1) % 24;
        updateAdjustAlarmDisplay(hSel);
      }
      // down
      else if (b == 2)
      {
        alarmHour = (alarmHour - 1) < 0 ? 23 : (alarmHour - 1);
        updateAdjustAlarmDisplay(hSel);
      }
    }
    // minutes
    else
    {
      // up
      if (b == 1)
      {
        alarmMinute = (alarmMinute + 1) % 60;
        updateAdjustAlarmDisplay(hSel);
      }
      // down
      else if (b == 2)
      {
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
  if (hours)
    displayIconText(alarmIcon, " " + (alarmHour < 10 ? "0" + String(alarmHour) : String(alarmHour)) + "h", 0);
  else
    displayIconText(alarmIcon, " " + (alarmMinute < 10 ? "0" + String(alarmMinute) : String(alarmMinute)) + "m", 0);
}

void adjustingTimer()
{
  bool hSel = true;
  updateAdjustTimerDisplay(hSel);

  while (true)
  {
    int b = -1;
    if (modeButton())
    {
      bool shortPress = modeButtonShort();

      if (!shortPress)
        break;
      b = 0;
    }
    else if (upButton())
    {
      while (upButton()) delay(5);
      b = 1;
    }
    else if (downButton())
    {
      while (downButton()) delay(5);
      b = 2;
    }

    // switch mode
    if (b == 0)
    {
      hSel = !hSel;
      updateAdjustTimerDisplay(hSel);
    }
      
        
    // hours
    if (hSel) 
    {
      // up
      if (b == 1)
      {
        timerHour = (timerHour + 1) % 3;
        updateAdjustTimerDisplay(hSel);
      }
      // down
      else if (b == 2)
      {
        timerHour = (timerHour - 1) < 0 ? 2 : (timerHour - 1);
        updateAdjustTimerDisplay(hSel);
      }
    }
    // minutes
    else
    {
      // up
      if (b == 1)
      {
        timerMinute = (timerMinute + 1) % 60;
        updateAdjustTimerDisplay(hSel);
      }
      // down
      else if (b == 2)
      {
        timerMinute = (timerMinute - 1) < 0 ? 59 : (timerMinute - 1);
        updateAdjustTimerDisplay(hSel);
      }
    }
    

    // wait for 5 milliseconds
    delay(5);
  }

  if (timerHour == 0 && timerMinute == 0)
    timerMinute = 1;
  timerActive = false;
}
void updateAdjustTimerDisplay(bool hours) 
{
  if (hours)
    displayIconText(timerIcon, " " + String(timerHour) + "h", 0);
  else
    displayIconText(timerIcon, " " + (timerMinute < 10 ? "0" + String(timerMinute) : String(timerMinute)) + "m", 0);
}