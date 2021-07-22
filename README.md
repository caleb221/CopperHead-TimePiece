# CopperHead-TimePiece
A 'Retro' ESP32 Based Smart watch using an HPDL-1414 and an .91'' OLED <br>**42x35mm**<br>
<img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/testPCB2.jpg"
     width ="250" height="250">
<img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/espTesterPCB1.jpg"
     width ="300" height="250" style="float:right">
# IN PROGRESS
    --> waiting on shipping and hardware testing
    --> WILL BE ABLE TO SOLDER ON MPU9250 SOON
    --> TEST MEMS MICROPHONE ANALOG CIRCUIT (read with I2S)
        
  Add personal Assistant with Mic using:
  <a href="https://www.survivingwithandroid.com/esp32-tensorflow-micro-speech-i2s-external-microphone/">ESP32-I2S WITH TENSORFLOW MICRO</a>

# Features
  Watch based around the ESP32 and HPDL1414. <br>
  Update time with NTP in known WiFi Areas (SSIDS/Passwords saved in SPIFFS JSON file) <br>
  32Khz Crystal for accuracy during deep sleep<br>
  Temperature and Humidity readings <br>
  Battery Charging and monitoring <br>
  MPU-950 Gyroscope/Accelerometer/Magenetometer (Compass)<br>
  MEMS Analog Microphone (to be used with TF-Lite Speech Recognition for wake words / personal assistant)<br>
  I2C/UART/SPI modular breakout for future sensors/ICs <br>
  Chrome Dino Game  <br>
  Space Invaders Game <br>
  Update WiFi networks and system time over a hosted website incase there are no known networks around and time is off.
   
# Schematic
<img src="https://github.com/caleb221/CopperHead-TimePiece/blob/main/img/Schematic_esp32SmartWatch_2021-07-22.png" width="700" height="500">  
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
     XC6203P332PR DC-DC Regulator (LDO)
     GMA3722H11-F42 MEMS Analog Microphone
     OPA344NA Analog Op Amp
     TYPE-C-31-M-17 USB-C Connector (6-Pin Power Only) 
     3.7v 320mAh/420mAh LiPo Battery
     ZH1.5 SMD Right Angle Connectors (4 and 2 pin female)  
     right angle tactile switch 
     PCB (from Gerber File attached)
     M3x4mm Screws (X6)
     10mm Brass Standoffs (X3)
     3D-Printed Case (STL file)
     Watch Strap (20mm Thick)
     External FTDI/CP2102 USB-TTL converter
     
 <a href ="http://www.farnell.com/datasheets/76528.pdf">HPDL-1414 Datasheet</a>

# Software
     The ESP32 holds a SPIFFS filesystem containing the following files:
               --> index.html (should erase when BLE works) 
               --> userConf.json (known WiFi/Passwords) user edit on upload/web interface
               --> sysTime.json (last updated time) user edit on upload/web interface
               --> highScores.json (high score info for games *UNTESTED*)
     The ESP32 spends most of its time in deep sleep, unless woken from the UI buttons
     Upon wakeup the ESP32 will update the OLED with month, day of week, battery level, temperature, and humidity.
     The HPDL-1414 is used for time in HH:MM format. "1259" = 12:59
     There is also a timer on the watch so that it cannot be awake for too long (always default to sleep)
     The Chrome 404 Dino Game (made for this watch / screen / UI)
     Space Invaders (made for this watch / screen / UI)
     
     
   <a href="https://github.com/caleb221/ArduinoChromeDinoGame">My Arduino  Chrome Dino Game</a> 
     
     
    
            
# Gerber / BOM
      check out the Manufacturing Files folder for these!
      --> Gerber_PCB_esp32SmartWatch_2.zip  2-Layer Board **DOES NOT HAVE MICROPHONE**
      --> Gerber_PCB_esp32SmartWatch_3.zip  4-Layer Board **MOST CURRENT**
      I got most normal components at LCSC (excluding those listed below)
      PCB made at JLCPCB
      --> I soldered on the components but I'm sure the SMT Assembly is an option given you match up the BOM
 <br>
 <a href="https://lcsc.com/">LCSC Components</a><br>
 <a href="https://jlcpcb.com/">JLCPCB Pcb Manufacturing</a>
 <br><br>
 
  **THINGS NOT BOUGHT FROM LCSC:** 
 
 <a href ="https://www.amazon.com/dp/B08215N9R8/ref=emc_b_5_t" >3.7v 320mAh LiPo (Amazon)</a>
 **OR**
 <a href = "https://www.adafruit.com/product/4236">3.7 420mAh Lipo (Adafruit)</a><br>
 <a href ="https://www.amazon.com/dp/B08NGL1PQ2?psc=1&ref=ppx_yo2_dt_b_product_details">0.91'' OLED Display (Amazon)</a><br>
 <a href ="https://www.amazon.com/dp/B076BNC876?psc=1&ref=ppx_yo2_dt_b_product_details"> 20mm Leather Watch Strap (Amazon) </a><br>
 <a href="https://www.amazon.com/Uxcell-a14050600ux1057-Female-Hexagon-Standoff/dp/B00NQ87PVK/ref=sr_1_1?dchild=1&keywords=Spacers&pd_rd_r=28e88884-33a1-49d9-b134-ab3fc683e599&pd_rd_w=Q4tF6&pd_rd_wg=61MQw&pf_rd_p=b4950e17-f2f6-494c-bba5-69a9d0aa3887&pf_rd_r=N3VR7BWXXMX8RAYR7R4Y&pid=FPrDztn&qid=1626448252&refinements=p_n_feature_twenty-three_browse-bin%3A19047482011%2Cp_n_feature_twenty_browse-bin%3A17420943011%2Cp_n_feature_twenty-eight_browse-bin%3A19043863011&s=industrial&sr=1-1"> Brass Standoffs (Amazon)</a><br>
 <a href ="https://www.amazon.com/10-M3-0-5-4mm-Countersunk-MonsterBolts/dp/B07983R7XC/ref=sr_1_1?dchild=1&keywords=Screws&pd_rd_r=509f2999-5a08-4e05-b24d-96c01ce9655b&pd_rd_w=dPIOh&pd_rd_wg=l7pkf&pf_rd_p=b4950e17-f2f6-494c-bba5-69a9d0aa3887&pf_rd_r=51BNB60JH1W9VNP5C807&pid=ONAxByd&qid=1626448556&refinements=p_n_feature_fourteen_browse-bin%3A11434052011%2Cp_n_feature_two_browse-bin%3A2292860011%2Cp_n_feature_twenty-eight_browse-bin%3A19043855011%2Cp_n_feature_thirteen_browse-bin%3A15245615011&s=industrial&sr=1-1">M3 x4mm black Screws (Amazon)</a><br>
 <a href ="https://www.ebay.com/itm/303806129405?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2060353.m2749.l2649"> EBAY HPDL-1414 (not guaranteed)</a><br>**EBAY AND AMAZON LINKS/PRODUCTS EXPIRE, YOU'LL NEED TO SEARCH FOR HPDL-1414 WHEREVER YOU NORMALLY SOURCE ODD COMPONENTS**<br> 
 
 
# TODO
     Get a stencil/ hot air blower so I can solder on the MPU-9250 <br>
     Make an addon with the modular breakouts (I2C/ UART)
               --> Raspberry pi 0 (with something cool)
               --> SD Card (SPI in Software, would need both ports)
               --> Geiger counter
               --> C02 Monitor
               --> Smart home / MQTT control (would need a UI sub directory added)
               --> Connect LoRa and control the smart farm! (this would take awhile)
# Would Be Nice:
     Waterproof case (hasunCast/PCB potting/ Resin)
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
