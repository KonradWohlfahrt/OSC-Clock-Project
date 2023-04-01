/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

// real (numbers) for the fast fourier transform (will be read by the microphone)
double _vReal[SAMPLES];
// imaginary (numbers) for the fast fourier transform
double _vImag[SAMPLES];
// the frequency which was extracted (values from 0(nothing) to 8(full))
int _data[XRES];
const int _adjustStyle[9] = { 0, 128, 192, 224, 240, 248, 252, 254, 255 };

// the fft class to perform the fast fourier transform: real, imaginary, samples, max. frequency
arduinoFFT _fft = arduinoFFT(_vReal, _vImag, SAMPLES, 40000);

void visualizer()
{
  lastRefreshTimestamp = millis();

  for (int i = 0; i < XRES; i++)
    _data[i] = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    _vReal[i] = 0;
    _vImag[i] = 0;
  } 

  while(true)
  {
    //break out of the while loop if a button is pressed
    if (anyButton())
    {
      while(anyButton())
        delay(5);
      break;
    }
    // check for alarm, timer and brightness
    if (millis() - lastRefreshTimestamp >= 1000)
    {
      checkAlarm();
      checkTimer();
      checkBrightness(true);

      lastRefreshTimestamp = millis();
    }

    // go through the samples
    for (int i = 0; i < SAMPLES; i++)
    {
      // save the reading from the microphone to the real array value
      _vReal[i] = analogRead(MIC);
      // set the imaginary array value to 0
      _vImag[i] = 0;
    }

    // remove the voltage offset
    _fft.DCRemoval();
    // create better results when using fast fourier transform.
    _fft.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    // run the fft
    _fft.Compute(FFT_FORWARD);
    // use the pytagoras equation (real, imaginary numbers) to get the frequency data (put into vReal)
    _fft.ComplexToMagnitude();
    
    // go through the screen x-axis
    for (int i = 0; i < XRES; i++)
    {
      // decrease the last value by one 
      _data[i] = (_data[i] - 1) < 0 ? 0 : (_data[i] - 1);

      // set the value for the frequency if it passed the noise filter
      int v = 0;
      if (_vReal[i] > noise)
        v = (int)(_vReal[i] / amplitude);

      // check if the new value is higher than the last one
      if (v > _data[i])
      {
        // set the new value
        _data[i] = (v > YRES - 1) ? (YRES - 1) : (v < 0 ? 0 : v);
      }

      // set the columns on the display
      mx.setColumn(XRES - 1 - i, styleArray[_data[i]]);
    }

    // wait for 5 milliseconds
    delay(5);
  }
}

void adjustAudioVisualizer()
{
  int selection = 0;
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
      selection = (selection + 1) % 2;
    }
    else if (b == 1 || b == 2)
    {
      // hour mode & up or down
      if (selection == 0)
      { 
        if (b == 1)
          amplitude = (amplitude + 100) > 900 ? 100 : (amplitude + 100);
        else
          amplitude = (amplitude - 100) < 100 ? 900 : (amplitude - 100);
      }
      // minutes mode & up or down
      else if (selection == 1)
      {
        if (b == 1)
          noise = (noise + 100) > 800 ? 0 : (noise + 100);
        else
          noise = (noise - 100) < 0 ? 800 : (noise - 100);
      }
    }

    updateAdjustingAudioVisualizer(selection);

    // wait for 5 milliseconds
    delay(5);
  }
}
void updateAdjustingAudioVisualizer(int selection)
{
  displayIcon(selection == 0 ? amplitudeIcon : noiseIcon, 0);

  // go through the samples
  for (int i = 0; i < SAMPLES; i++)
  {
    // save the reading from the microphone to the real array value
    _vReal[i] = analogRead(MIC);
    // set the imaginary array value to 0
    _vImag[i] = 0;
  }

  // remove the voltage offset
  _fft.DCRemoval();
  // create better results when using fast fourier transform.
  _fft.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  // run the fft
  _fft.Compute(FFT_FORWARD);
  // use the pytagoras equation (real, imaginary numbers) to get the frequency data (put into vReal)
  _fft.ComplexToMagnitude();
  
  // go through the screen x-axis
  for (int i = 0; i < 16; i++)
  {
    // decrease the last value by one 
    _data[i] = (_data[i] - 1) < 0 ? 0 : (_data[i] - 1);

    // set the value for the frequency if it passed the noise filter
    int v = 0;
    if (_vReal[i] > noise)
      v = (int)(_vReal[i] / amplitude);

    // check if the new value is higher than the last one
    if (v > _data[i])
    {
      // set the new value
      _data[i] = (v > 7) ? 7 : (v < 0 ? 0 : v);
    }

    // set the columns on the display
    mx.setColumn(15 - i, styleArray[_data[i]]);
  }

  for (int i = 23; i >= 16; i--)
  {
    byte v = 0;
    if (selection == 0)
      v = (byte)map(amplitude, 100, 900, 0, 8);
    else
      v = (byte)map(noise, 0, 800, 0, 8);
    mx.setColumn(i, _adjustStyle[v]);
  }
}