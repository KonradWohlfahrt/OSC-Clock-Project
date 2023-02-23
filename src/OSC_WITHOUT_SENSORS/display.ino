void displayText(String text, int transform)
{
  // the length of the text
  int len = text.length();
  // the index of the current character
  int textIndex = 0 + transform;

  // go through the display columns
  for (int i = 31; i >= 0; i--)
  {
    // is the index not outside of the range
    if (textIndex < len)
    {
      if (textIndex >= 0)
      {
        // set the current character
        int width = mx.setChar(i, text[textIndex]);
        i -= width;
        mx.setColumn(i, 0);
      }
      else
        mx.setColumn(i, 0);
      
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
  // the length of the text
  int len = text.length();
  // the index of the current character
  int textIndex = 0 + transform;

  // go through the display columns
  for (int i = 23; i >= 0; i--)
  {
    // is the index not outside of the range
    if (textIndex < len)
    {
      if (textIndex >= 0)
      {
        // set the current character
        int width = mx.setChar(i, text[textIndex]);
        i -= width;
        mx.setColumn(i, 0);
      }
      else
        mx.setColumn(i, 0);

      // swap to next character
      textIndex++;
    }
    else
      mx.setColumn(i, 0);
  }

  // display the icon
  displayIcon(icon, 0);
}

void displayScrollText(String text, int textDelay)
{
  int textLen = text.length();

  byte charWidth;
  byte cBuf[8];

  mx.clear();

  for (int i = 0; i < textLen + 32; i++)
  {
    if (i < textLen)
      charWidth = mx.getChar(text[i], sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
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

int currentCharLength = 0;
int currentCharIndex = 0;
int currentTextIndex = 0;
byte currentCharBuffer[8];
String currentDisplayText = "";

void resetScrollText()
{
  currentTextIndex = 0;
  currentCharIndex = 0;
  currentCharLength = 0;
}
void setScrollText(String t)
{
  resetScrollText();
  mx.clear();
  currentDisplayText = t;
}

void updateScrollText()
{
  mx.transform(MD_MAX72XX::TSL);
  bool hasChar = currentCharIndex < currentCharLength;

  if (hasChar)
  {
    mx.setColumn(0, currentCharBuffer[currentCharIndex]);
    currentCharIndex++;
  }
  else if (currentTextIndex < currentDisplayText.length())
  {
    currentCharIndex = 0;
    currentCharLength = mx.getChar(currentDisplayText[currentTextIndex], sizeof(currentCharBuffer) / sizeof(currentCharBuffer[0]), currentCharBuffer);
    currentTextIndex++;
  }
  else 
  {
    currentTextIndex++;
    if (currentTextIndex >= currentDisplayText.length() + 32)
      resetScrollText();
  }
}