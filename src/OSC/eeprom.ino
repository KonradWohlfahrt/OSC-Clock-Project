/*
  OSC-PROJECT-FILES
  Created by Donut Studio and Pr0metheuz_, April 01, 2023.
  Released into the public domain.
*/

// load the ssid, password and weatherapikey from the eeprom
void readEEPROM()
{
  // read ssid at 0 with maximal length of 32
  ssid = readEEPROMData(0, 32);

  // read ssid at 32 with maximal length of 32
  password = readEEPROMData(32, 32);

  // read ssid at 64 with maximal length of 64
  weatherAPIKey = readEEPROMData(64, 64);
}
// save the ssid, password and weatherapikey to the eeprom
void saveEEPROM(String _ssid, String _password, String _weatherAPIKey)
{
  // save ssid at 0 with maximal length of 32
  saveEEPROMData(_ssid, 0, 32);
  // save password at 32 with maximal length of 32
  saveEEPROMData(_password, 32, 32);
  // save weather api key at 64 with maximal length of 64
  saveEEPROMData(_weatherAPIKey, 64, 64);
}

// save a string to a specific adress on the eeprom
bool saveEEPROMData(String _data, int adressStart, int length)
{
  int dataLen = _data.length();
  if (dataLen > 0)
  {
    for (int i = 0; i < length; ++i)
    {
      if (i < dataLen)
        EEPROM.write(adressStart + i, _data[i]);
      else
        EEPROM.write(adressStart + i, 0);
    }
    EEPROM.commit();

    return true;
  }
  
  return false;
}
// read a string from a specific adress on the eeprom
String readEEPROMData(int adressStart, int length)
{ 
  String _data = "";
  int len = adressStart + length;
  for (int i = adressStart; i < len; ++i)
  {
    byte d = EEPROM.read(i);
    if (d != 0)
      _data += char(d);
  }
  return _data;
}
// clear the eeprom
void clearEEPROM()
{
  for (int i = 0; i < 96; ++i)
    EEPROM.write(i, 0);
}