# OSC-Clock-Project
In this repository you will find the source code for the diy smart home clock OSC.


# Folder structure
- the folder `pcb` contains the .zip file of the pcb (can be ordered by pcb fabrication companies)
- the folder `src` contains the source code for the different clock versions
    - `/OSC_DISPLAY_PANEL` contains the source code of the display panel: display your own text via a webserver
    - `/OSC_WITHOUT_SENSORS` contains the source code of the clock without the dht11 and max4466 sensor

# Libraries
You will need multiple libraries to compile the source code:
- MD_MAX72xx
- ESP8266 board manager
- NTPClient
- ArduinoJSON (! v5.13.5 !)
- DonutStudioMillisTime ([Millis Time](https://github.com/Donut-Studio/Arduino-Millis-Time-Library))
- DHT sensor library

# CREDITS
OSC-PROJECT-FILES
Created by Donut Studio and Pr0metheuz_, January 23, 2023.
Released into the public domain.