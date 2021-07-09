# CopperHead-TimePiece
A 'Retro' ESP32 Based Smart watch using an HPDL-1414 and an .91'' OLED <br>**42x35mm**<br>
<img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/testPCB1.jpg"
     width ="250" height="250">
<img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/espTesterPCB1.jpg"
     width ="300" height="250" style="float:right">
# IN PROGRESS
    --> waiting on shipping and hardware testing

# Features
  Watch based around the ESP32 and HPDL1414. <br>
  Update time with NTP in known WiFi Areas (SSIDS/Passwords saved in SPIFFS JSON file) <br>
  32Khz Crystal for accuracy during deep sleep<br>
  Temperature and Humidity readings <br>
  Battery Charging and monitoring <br>
  I2C/UART modular breakout for future sensors/ICs <br>
  Chrome Dino Game  <br>
  Space Invaders Game <br>
   **footprint is there but currently no way to solder on MPU-9250** <br>
   Update WiFi networks and time over a hosted website incase there are no known networks around and time is off.
   
# Schematic
<img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/Schematic_esp32SmartWatch_2021-07-04.png" width="650" height="500">

# Gerber / BOM
       check out the Manufacturing Files for these!
       I got most components at LCSC (excluding HPDL-1414)
       PCB made at JLCPCB
  <img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/cleanGreenFRONT.svg" width="200" height ="200">
  <img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/cleanGreenBACK.svg" width="200" height ="200" style="float:right">
       
# Hardware
  ESP32 Wrover-B (4 or 8Mb) <br>
  HPDL-1414 Segmented Display  <br>
  0.91'' OLED Display <br>
  SHT20 Temperature/Humidity Sensor <br>
  MPU-9250 Gyroscope/Accelerometer/Magnetometer <br>
  Tactile Switches (3) <br>
  Various SMD Resistors/Capacitors <br>
  32.768kHz ±20ppm Crystal <br>
  TP4056 <br>
  DW01A   <br>
  FS8205A
  HX4004 DC-DC Step up (LDO) <br>
  TPS62203DBVR DC-DC Step Down (buck) <br>
  10uH Inductor <br>
  3.7 LiPo Battery <br>
  ZH1.5 SMD Right Angle Connectors (4 and 2 pin female)  <br>
  right angle tactile switch <br>
  PCB (from Gerber File attached)<br>
  M3x4mm Screws (X6)<br>
  10mm Brass Standoffs (X3)<br>
  3D-Printed Case <br>
  Watch Strap (20mm Thick)<br>
  External FTDI/CP2102 USB-TTL converter<br>
# TODO
     Get a stencil/ hot air blower so I can solder on the MPU-9250 <br>
     Make an addon with the modular breakouts (I2C/ UART)
               --> Raspberry pi 0 (with something cool idk, personal assistant?)
               --> SD Card (SPI in Software, would need both ports)
               --> Geiger counter
               --> C02 Monitor
               --> Smart home / MQTT control (would need a UI sub directory added)
               --> Connect LoRa and control the smart farm! (this would take awhile)
# Would Be Nice:
     Waterproof case
     Glass over screens
     smaller PCB (if possible)
     bigger battery
     move buttons to bottom layer / change to right angled tactile buttons
     Use external RTC (PCF8563M / C434478)
     Android Application (web UI takes place of this for now)
     Use BLE Server implementation (commented out at bottom of code) --> 
          accepts JSON data and updates internal filesystem
          Web Server uses same functions and tested, BLE with JSON is UNTESTED
