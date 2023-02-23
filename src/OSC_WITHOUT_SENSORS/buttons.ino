/* --- BUTTONS --- */
bool upButton() { return digitalRead(12) == 0; }
bool downButton() { return digitalRead(5) == 0; }
bool modeButton() { return digitalRead(3) == 0; }
bool anyButton() { return upButton() || downButton() || modeButton(); }
bool anySelectButton() { return upButton() || downButton(); }

bool upButtonShort()
{
  startPress = millis();
  bool shortPress = true;

  // wait for the user to release the button
  while (upButton()) 
  {
    if (shortPress && millis() - startPress >= LONGPRESSTIME)
    {
      shortPress = false;
      setBuzzer(25);
    }
    delay(5);
  }
  return shortPress;
}
bool downButtonShort()
{
  startPress = millis();
  bool shortPress = true;

  // wait for the user to release the button
  while (downButton()) 
  {
    if (shortPress && millis() - startPress >= LONGPRESSTIME)
    {
      shortPress = false;
      setBuzzer(25);
    }
    delay(5);
  }
  return shortPress;
}
bool modeButtonShort()
{
  startPress = millis();
  bool shortPress = true;

  // wait for the user to release the button
  while (modeButton()) 
  {
    if (shortPress && millis() - startPress >= LONGPRESSTIME)
    {
      shortPress = false;
      setBuzzer(25);
    }
      
    delay(5);
  }
  return shortPress;
}