/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

int _currentCharLength = 0;
int _currentCharIndex = 0;
int _currentTextIndex = 0;
byte _currentCharBuffer[8];
String _currentDisplayText = "";
byte _currentIcon[8];


/* --- SCROLL TEXT ---*/
void resetScrollText()
{
  _currentTextIndex = 0;
  _currentCharIndex = 0;
  _currentCharLength = 0;
}
void setScrollText(String t)
{
  resetScrollText();
  mx.clear();
  _currentDisplayText = fontReplace(t);
}
void shiftScrollText()
{
  mx.transform(MD_MAX72XX::TSL);
  bool hasChar = _currentCharIndex < _currentCharLength;

  if (hasChar)
  {
    mx.setColumn(0, _currentCharBuffer[_currentCharIndex]);
    _currentCharIndex++;
  }
  else if (_currentTextIndex < _currentDisplayText.length())
  {
    if (_currentDisplayText[_currentTextIndex] == 195 || _currentDisplayText[_currentTextIndex] == 194)
      _currentTextIndex++;

    _currentCharIndex = 0;
    _currentCharLength = mx.getChar(_currentDisplayText[_currentTextIndex], sizeof(_currentCharBuffer) / sizeof(_currentCharBuffer[0]), _currentCharBuffer);
    _currentTextIndex++;
  }
  else 
  {
    _currentTextIndex++;
    if (_currentTextIndex >= _currentDisplayText.length() + 32)
      resetScrollText();
  }
}


/* --- SCROLL TEXT WITH ICON --- */
void resetIconText()
{
  _currentTextIndex = 0;
  _currentCharIndex = 0;
  _currentCharLength = 0;
}
void setIconText(byte b[8], String t)
{
  resetIconText();
  mx.clear();

  for (int i = 0; i < 8; i++)
    _currentIcon[i] = b[i];
  _currentDisplayText = fontReplace(t);
}
void shiftIconText()
{
  mx.transform(0, 2, MD_MAX72XX::TSL);
  bool hasChar = _currentCharIndex < _currentCharLength;
  if (hasChar)
  {
    mx.setColumn(0, _currentCharBuffer[_currentCharIndex]);
    _currentCharIndex++;
  }
  else if (_currentTextIndex < _currentDisplayText.length())
  {
    if (_currentDisplayText[_currentTextIndex] == 195 || _currentDisplayText[_currentTextIndex] == 194)
      _currentTextIndex++;

    _currentCharIndex = 0;
    _currentCharLength = mx.getChar(_currentDisplayText[_currentTextIndex], sizeof(_currentCharBuffer) / sizeof(_currentCharBuffer[0]), _currentCharBuffer);
    _currentTextIndex++;
  }
  else 
  {
    _currentTextIndex++;
    if (_currentDisplayText[_currentTextIndex] == 195 || _currentDisplayText[_currentTextIndex] == 194)
      _currentTextIndex++;
    if (_currentTextIndex >= _currentDisplayText.length() + 24)
      resetIconText();
  }

  displayIcon(_currentIcon, 0);
}

// scroll text (interrupts code)
void displayScrollText(String text, int textDelay)
{
  text = fontReplace(text);

  int textLen = text.length();

  byte charWidth;
  byte cBuf[8];

  mx.clear();

  for (int i = 0; i < textLen + 32; i++)
  {
    if (i < textLen)
    {
      if (text[i] == 195 || text[i] == 194)
        i++;
      charWidth = mx.getChar(text[i], sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
    }
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

// set the time
void showSetTime(int first, int second, bool hasSelectedFirst, char seperator, int transform)
{
  String text = (first < 10) ? "0" + String(first) : String(first);
  text += seperator;
  text += (second < 10) ? "0" + String(second) : String(second);

  // the length of the text
  int len = text.length();
  // the index of the current character
  int textIndex = transform;

  int underlineStart;
  int underlineEnd;

  // go through the display columns
  for (int i = 31; i >= 0; i--)
  {
    // is the index not outside of the range
    if (textIndex < len)
    {
      if (textIndex >= 0)
      {
        if ((hasSelectedFirst && textIndex == 0) || (!hasSelectedFirst && textIndex == 3))
          underlineStart = i;

        // set the current character
        int width = mx.setChar(i, text[textIndex]);
        i -= width;
        mx.setColumn(i, 0);

        if ((hasSelectedFirst && textIndex == 1) || (!hasSelectedFirst && textIndex == 4))
          underlineEnd = i + 1;
      }
      else
        mx.setColumn(i, 0);
      
      // swap to next character
      textIndex++;
    }
    else
      mx.setColumn(i, 0);
  }

  // draw the underline
  for (int i = underlineStart; i >= underlineEnd; i--)
    mx.setPoint(7, i, true);
}

/* --- OTHER DISPLAY SETTINGS ---*/
void displayText(String text, int transform)
{
  text = fontReplace(text);

  // the length of the text
  int len = text.length();
  // the index of the current character
  int textIndex = transform;

  // go through the display columns
  for (int i = 31; i >= 0; i--)
  {
    // is the index not outside of the range
    if (textIndex < len)
    {
      if (textIndex >= 0 && text[textIndex] != 195 && text[textIndex] != 194)
      {
        // set the current character
        int width = mx.setChar(i, text[textIndex]);
        i -= width;
        mx.setColumn(i, 0);
      }
      else
      {
        if (text[textIndex] == 195 || text[textIndex] == 194)
          i++;
        mx.setColumn(i, 0);
      }
      
      // swap to next character
      textIndex++;
    }
    else
      mx.setColumn(i, 0);
  }
}
void displayIcon(byte icon[8], int transform)
{
  // go through the left display columns
  for(int i = 31; i >= 24; i--)
  {
    // get the index relativ to the transformation
    int index = 31 - i + transform;
    // set the icon or set nothing
    if (index < 8 && index >= 0)
      mx.setColumn(i, icon[index]);
    else
      mx.setColumn(i, 0);
  }
}
void displayIconText(byte icon[8], String text, int transform)
{
  text = fontReplace(text);

  // the length of the text
  int len = text.length();
  // the index of the current character
  int textIndex = transform;

  // go through the display columns
  for (int i = 23; i >= 0; i--)
  {
    // is the index not outside of the range
    if (textIndex < len)
    {
      if (textIndex >= 0 && text[textIndex] != 195 && text[textIndex] != 194)
      {
        // set the current character
        int width = mx.setChar(i, text[textIndex]);
        i -= width;
        mx.setColumn(i, 0);
      }
      else
      {
        if (text[textIndex] == 195 || text[textIndex] == 194)
          i++;
        mx.setColumn(i, 0);
      }

      // swap to next character
      textIndex++;
    }
    else
      mx.setColumn(i, 0);
  }

  // display the icon
  displayIcon(icon, 0);
}

String fontReplace(String s)
{
  // if some characters don't show up correctly, added them here...
  s.replace('Ä', (char)196);
  s.replace('ä', (char)228);
  s.replace('Ö', (char)214);
  s.replace('ö', (char)246);
  s.replace('Ü', (char)220);
  s.replace('ü', (char)252);
  s.replace('ß', (char)130);
  s.replace('°', (char)144);
  return s;
}