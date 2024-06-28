# OSC-Clock-Project
In this repository you will find the source code for the diy smart home clock OSC.
The instructable can be found here: [Instructables.com](https://www.instructables.com/DIY-Smart-Home-Clock-OSC/)

# Folder structure
- the folder `documents/` contains info files of the clock
- the folder `pcb/` contains the .zip file of the pcb (can be ordered by pcb fabrication companies)
- the folder `src/` contains the source code for the different clock versions
    - `/OSC/` contains the source code of the clock with the two sensors
    - `/OSC_DISPLAY_PANEL/` contains the source code of the display panel: display your own text via a webserver
    - `/OSC_WITHOUT_SENSORS/` contains the source code of the clock without the dht11 and max4466 sensor
- the folder `stl/` contains the 3d files for the clock to print

# Libraries
You will need multiple libraries to compile the source code:
- ESP8266 board manager
- MD_MAX72xx
- NTPClient
- ArduinoJSON (! v5.13.4 !)
- DonutStudioMillisTime ([Millis Time](https://github.com/KonradWohlfahrt/Arduino-Millis-Time-Library))
- DonutStudioTimer ([Timer](https://github.com/KonradWohlfahrt/Arduino-Timer-Library))
- DonutStudioStopwatch ([Stopwatch](https://github.com/KonradWohlfahrt/Arduino-Stopwatch-Library))
- Arduino FFT
- DHT sensor library

# CREDITS
OSC-PROJECT-FILES
Created by Donut Studio and Pr0metheuz_, April 01, 2023.
Released into the public domain.
