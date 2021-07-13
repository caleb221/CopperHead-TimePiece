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
      check out the Manufacturing Files folder for these!
      I got most normal components at LCSC (excluding those listed below)
      PCB made at JLCPCB
 <br>
 <a href="https://lcsc.com/">LCSC</a><br>
 <a href="https://jlcpcb.com/">JLCPCB</a>
 <br><br>
 
  **THINGS NOT BOUGHT FROM LCSC:** 
 
 <a href ="https://www.amazon.com/dp/B08215N9R8/ref=emc_b_5_t" >3.7v 320mAh LiPo on Amazon</a><br>
 <a href ="https://www.amazon.com/dp/B08NGL1PQ2?psc=1&ref=ppx_yo2_dt_b_product_details">0.91'' OLED Display on Amazon</a><br>
 <a href ="https://www.amazon.com/dp/B076BNC876?psc=1&ref=ppx_yo2_dt_b_product_details"> 20mm Leather Watch Strapon Amazon </a><br>
 <a href ="https://www.ebay.com/itm/303806129405?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2060353.m2749.l2649"> EBAY HPDL-1414 (not guaranteed)</a><br>**EBAY AND AMAZON LINKS/PRODUCTS EXPIRE, YOU'LL NEED TO SEARCH FOR HPDL-1414 WHEREVER YOU NORMALLY SOURCE ODD COMPONENTS**<br>
  
  <img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/cleanGreenFRONT.svg" width="200" height ="200">
  <img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/cleanGreenBACK.svg" width="200" height ="200" style="float:right">
       
# Hardware
     ESP32 Wrover-B (4 or 8Mb) 
     HPDL-1414 Segmented Display  
      0.91'' OLED Display 
     SHT20 Temperature/Humidity Sensor 
     MPU-9250 Gyroscope/Accelerometer/Magnetometer 
     Tactile Switches (3) 
     Various SMD Resistors/Capacitors (0608 Resistors)
     32.768kHz ±20ppm Crystal
     TP4056 
     DW01A  
     FS8205A
     HX4004 DC-DC Step up (LDO) 
     TPS62203DBVR DC-DC Step Down (buck)
     10uH Inductor
     3.7v 320mAh LiPo Battery
     ZH1.5 SMD Right Angle Connectors (4 and 2 pin female)  
     right angle tactile switch 
     PCB (from Gerber File attached)
     M3x4mm Screws (X6)
     10mm Brass Standoffs (X3)
     3D-Printed Case 
     Watch Strap (20mm Thick)
     External FTDI/CP2102 USB-TTL converter
     
 <a href ="http://www.farnell.com/datasheets/76528.pdf">HPDL-1414 Datasheet</a>
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
     bigger battery (needs to fit inside the case!)
     move buttons to bottom layer / change to right angled tactile buttons
     Use external RTC (DS3231 /C9866) OR (PCF8563M / C434478)
                      (part  / LCSC)
     Android Application (web UI takes place of this for now)
     Use BLE Server implementation (commented out at bottom of code) --> 
          accepts JSON data and updates internal filesystem
          Web Server uses same functions and tested, BLE with JSON is UNTESTED
