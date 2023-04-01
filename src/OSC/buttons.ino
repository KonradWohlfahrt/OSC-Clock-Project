/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

unsigned long _startedPressTimestamp;

bool upButton() { return digitalRead(12) == 0; }
bool downButton() { return digitalRead(5) == 0; }
bool modeButton() { return digitalRead(3) == 0; }
bool anyButton() { return upButton() || downButton() || modeButton(); }

bool modeButtonShort(bool makeSound)
{
  // set the timestamp
  _startedPressTimestamp = millis();
  // set the boolean representive for the press type
  bool shortPress = true;

  // wait for the user to release the button
  while (modeButton()) 
  {
    // should the press should be counted as a long press
    if (shortPress && millis() - _startedPressTimestamp >= LONGPRESSTIME)
    {
      // set the press type to long
      shortPress = false;

      // make a sound
      if (makeSound)
        setBuzzer(25);
    }
    // wait for 5 milliseconds
    delay(5);
  }
  // return the press type
  return shortPress;
}