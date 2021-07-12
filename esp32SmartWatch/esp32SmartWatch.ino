//CpHd's TimePiece
// if WiFi is known auto update
// SPIFFS
// sysTime.json -->
// userConf.json --> saved SSID/Passwords
 //       --> probably not a good idea to store plaintext passwords
// highScores.json --> game highscores
// 
// 0.91'' 128x32 I2C OLED
// HDLP1414 DIP12 Segmented LED Display
// IMU --> X,Y,Z, Compass (HARD TO SOLDER)
// SHT20 --> Temperature / Humidity I2C
//
// dinoGame.ino --> Arduino version of Chrome Dino Game (made for .91 screen)
// spaceInvaders.ino --> Attached at bottom

/*
=======================================
          PINOUT
=======================================
IO - CONNECTION
02 - HPDL1414
04 - HPDL1414
05 - HPDL1414
12 - HPDL1414
13 - HPDL1414
14 - HPDL1414
15 - HPDL1414
18 - HPDL1414
19 - HPDL1414
21 - SDA
TX - TX
RX - RX
22 - SCL
23 - HPDL1414
25 - BUTTON SELECT
27 - FREE/NC
26 - BATTERY MONITOR
32 - CRYSTAL
33 - CRYSTAL
34 - BUTTON DOWN
35 - BUTTON UP 
 */
 
#include <Wire.h>
#include <HPDL1414.h>
#include <driver/adc.h>
#include "uFire_SHT20.h"

//#include "MPU9250.h"

#include<stdio.h>
#include "U8g2lib.h"
#include "SPIFFS.h"
#include <soc/rtc.h>
#include<time.h>
#include "time.h"

//connectivity
//#include<BLEDevice.h>
//#include<BLEUtils.h>
//#include<BLEServer.h>
#include<WiFi.h>
#include<WebServer.h>
#include<WiFiUdp.h>
#include<ArduinoJson.h>
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
//#include <ESPmDNS.h>

//HARDWARE
#define BTN_UP 35
#define BTN_SL 34
#define BTN_DN 32


#define BTN_BTMASKOLD 0xD00000000
#define BTN_BTMASK 0xC20000000
//Binary mask:
//110100000000000000000000000000000000 OLD
//110000100000000000000000000000000000 NEW
#define BATT_ADC 33

//HPDL1414:

//BOARD:    D5,D4,W,A1,A0
//ESP:      14,13,12,15,2
//        ===================
//          D0,D1,D2,D3,D6
//          4, 5, 18,19,23

const byte dataPins[7] = {4, 5, 18, 19,13,14,23}; // Segment data pins: D0 - D6
const byte addrPins[2] = {15, 2}; // Segment address pins: A0, A1
const byte wrenPins[] = {12}; // Write Enable pins (left to right)


//https://www.uuidgenerator.net/

//Temperature read Characteristic: 0x2A1C
//Battery: 0x180F
//User Data: 0x181C 
//0x2A08 Date Time

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 30//Seconds, update on the minute

#define CALIBRATE_ONE(cali_clk) calibrate_one(cali_clk, #cali_clk)

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
//.91 -->> SSD1306
HPDL1414 dopeScreen(dataPins, addrPins, wrenPins, sizeof(wrenPins));
uFire_SHT20 sht20;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;// For UTC -5.00 : -5 * 60 * 60 : -18000;
const int   daylightOffset_sec = 3600;

//char bleDataIn[300];
AsyncWebServer server(80);
bool didWebUpdate=false;
bool isWebServing=false;
//SENSOR DATA
float temperature;
float humidity;
float battLevel;

//GAME CONSTANTS
#define IMG_WIDTH 8
#define IMG_HEIGHT 8
#define IMG_SIZE 8
#define HEIGHT 32 //pixel screen height
#define DNR_CRTL 15 //Control Button Pin 
int ENEMY_START = 100;//X Starting for enemy rendering
int WIDTH=128;//pixel screen width
int FLOOR_LEVEL=22;
int JUMP_PIXELS=5;
int ENEMY_PAD = 20;

//============================================================================
//                   JSON/SPIFFS FILE SYSTEM HANDLING
//============================================================================

 bool updateJsonTime(int yr,int mnt,int dy,int hr, int minute,int sec)
{
  if(!SPIFFS.begin(true)){
            u8g2.clearBuffer();
        u8g2.drawStr(5,10,"SPIFFS");
        u8g2.drawStr(5,18,"ERROR X(");
        u8g2.sendBuffer(); 
        delay(3000);
    return false;
  }
  File rawFile = SPIFFS.open("/sysTime.json","w");
  if(!rawFile){
                u8g2.clearBuffer();
        u8g2.drawStr(5,10,"JSON SPIFFS");
        u8g2.drawStr(5,18,"ERROR X(");
        u8g2.sendBuffer(); 
        delay(3000);
         return false;
  }
     
  char buffer1[124];
  DynamicJsonDocument updateTimeDoc(124);
  updateTimeDoc["yr"]=yr;
  updateTimeDoc["mnt"]=mnt;
  updateTimeDoc["dy"]=dy;
  updateTimeDoc["hr"]=hr;
  updateTimeDoc["minute"]=minute;
  updateTimeDoc["sec"]=sec;
  serializeJson(updateTimeDoc,buffer1);
  rawFile.print(buffer1);
  rawFile.close();
  return true;
 }
 
bool updateJsonConfig(const char* newSSID,const char* newPWD)
{
  
  if(!SPIFFS.begin(true))     
      return false;
 File rawConf = SPIFFS.open("/userConf.json","r");
  
  if(!rawConf)
    return false;

    //CHANGE TO MATCH ARRAY
  StaticJsonDocument<1024> conf;
    StaticJsonDocument<1024> newConf;

  char buffer1[1024];
  
  DeserializationError err = deserializeJson(conf,rawConf);
  if(err)
    return false;
  JsonArray wIds = conf["W"].as<JsonArray>();
  JsonArray pIds = conf["P"].as<JsonArray>();
  
  wIds.add(newSSID);
  pIds.add(newPWD);
  
  rawConf.close();
  
  File rewriteFile = SPIFFS.open("/userConf.json","w");

  serializeJson(conf,buffer1);
  rewriteFile.print(buffer1);
  rewriteFile.close();
  return true;
}


void updateFromJSON()
{
    if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return ;
  }
  
  File rawFile = SPIFFS.open("/sysTime.json");
  
  if(!rawFile)
      return;
  StaticJsonDocument<256> jsonTime;
  DeserializationError err = deserializeJson(jsonTime,rawFile);
  
  if(err)
     Serial.println("JSON TIME OPEN ERR");

 int year=jsonTime["yr"];
 int month= jsonTime["mnt"];
 int day= jsonTime["dy"];
 int hour=jsonTime["hr"];
 int minute=jsonTime["minute"];
 int second=jsonTime["sec"];
  
  struct tm newTimeinfo;
  newTimeinfo.tm_year=year;
  newTimeinfo.tm_mon=month;
  newTimeinfo.tm_mday=day;
  newTimeinfo.tm_hour=hour;
  newTimeinfo.tm_min=minute;
  newTimeinfo.tm_sec=second;
  time_t tjson = mktime(&newTimeinfo); 
  //struct timeval updatr ={.tv_sec = tjson};

 
 struct tm * timeinfo;
 time_t t;
 timeval tval;
 gettimeofday(&tval, NULL);
  //convert to time values we can actually use
  t = tval.tv_sec;           //seconds since epoch
  //timeinfo = localtime(&t);  //time struct


if(t<tjson)//make sure JSON is more current
 updateSysTime(year,month,day,hour,minute,second);
//else //make JSON more current
  //updateJsonTime(year,month,day,hour,minute,second);

}

void updateSysTime(int year,int month,int day,int hour,int minute,int seconds)
{
  setenv("TZ", "EST5EDT4,M3.2.0/02:00:00,M11.1.0/02:00:00", 1);
  tzset();
  struct tm newTimeinfo;
  newTimeinfo.tm_year=year;
  newTimeinfo.tm_mon=month;
  newTimeinfo.tm_mday=day;
  newTimeinfo.tm_hour=hour;
  newTimeinfo.tm_min=minute;
  newTimeinfo.tm_sec=seconds;
/*
  Serial.print("\n======TIME UPDATER=====\n");
  Serial.print(year);
  Serial.print("  ");
  Serial.print(month);
  Serial.print("  ");
  Serial.print(hour);
  Serial.print(" : ");
  Serial.print(minute);
  Serial.println("\n==============");
*/
  time_t t = mktime(&newTimeinfo); 
  struct timeval updatr ={.tv_sec = t};
  settimeofday(&updatr,NULL);
}

void stopWiFi()
{
     //WiFi.isEnabled()
     WiFi.disconnect(true);
     WiFi.mode(WIFI_OFF);
     u8g2.clearBuffer();
     u8g2.drawStr(5,10,"Turning WiFi");
     u8g2.drawStr(5,20,"OFF");
     u8g2.sendBuffer();
     delay(3000);  
isWebServing=false;
}


void tryWiFi_time()
{

  //make sure we have the antenna!!
  //stopBLE();
  delay(100);

  if(!SPIFFS.begin(true))
  {
        u8g2.clearBuffer();
        u8g2.drawStr(10,10,"SPIFFS  ERR");
        u8g2.sendBuffer();    
        delay(3000);
        return;  
  }
  
    File rawConf = SPIFFS.open("/userConf.json");
  
    if(!rawConf)
    {
        u8g2.clearBuffer();
        u8g2.drawStr(10,10,"JSON  ERR");
        u8g2.sendBuffer();    
        delay(3000);
        return;    
    }

    //CHANGE TO MATCH ARRAY
  StaticJsonDocument<256> conf;
  DeserializationError err = deserializeJson(conf,rawConf);
  
      if(err)
      {
        u8g2.clearBuffer();
        u8g2.drawStr(10,10,"JSON  ERR");
        u8g2.sendBuffer();
        delay(3000);    
      }
      
  JsonArray wIds = conf["W"].as<JsonArray>();
  JsonArray pIds = conf["P"].as<JsonArray>();
  int pCounter=0;
  size_t pSize = pIds.size();
  for(JsonVariant wNow :wIds)
  {
    const char* w = wNow.as<char*>();
    const char* p = pIds[pCounter].as<char*>();

    int connectCount=0;
    WiFi.begin(w,p);
      while (connectCount < 5) 
      {
        delay(500);
        connectCount++;
        u8g2.clearBuffer();
        u8g2.drawStr(0,10,"Connecting to..");
        u8g2.drawStr(20,18,w);
        u8g2.sendBuffer();
        }
      pCounter++;
      if(WiFi.status() == WL_CONNECTED)
       break;
  } 
  rawConf.close();
      
  if(WiFi.status() == WL_CONNECTED) 
  {   
    struct tm timeinfo;

    Serial.println("getting NTP Time");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    char* myIP = (char *) WiFi.localIP().toString().c_str();
        u8g2.clearBuffer();
        u8g2.drawStr(5,10,"Connected at");
        u8g2.drawStr(5,18,myIP);
        u8g2.drawStr(5,24,"getting NTP Time");
        u8g2.sendBuffer();
        delay(2000);
  
  while(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(100);
    }//make sure NTP updates itself


  updateSysTime( (timeinfo.tm_year),
                  (timeinfo.tm_mon),
                  (timeinfo.tm_mday),
                  timeinfo.tm_hour,
                  timeinfo.tm_min,
                  timeinfo.tm_sec
            );
   bool didUpdate = updateJsonTime((timeinfo.tm_year),
                  (timeinfo.tm_mon),
                  (timeinfo.tm_mday),
                  timeinfo.tm_hour,
                  timeinfo.tm_min,
                  timeinfo.tm_sec
            );

    char* updateStat="NO";
    if(didUpdate)
      updateStat="TRU";
        u8g2.clearBuffer();
        u8g2.drawStr(10,18,"NTP Time Updated:");
        u8g2.drawStr(10,24,updateStat);
        u8g2.sendBuffer();
        delay(2000);
  }
  else
  {
        u8g2.clearBuffer();
        u8g2.drawStr(5,10,"No WiFi Found");
        u8g2.drawStr(5,18,"unable to update");
        u8g2.drawStr(5,24,"find a known network! X(");
        u8g2.sendBuffer();
        delay(5000);
  
  }
stopWiFi(); 
}

void espSleep()
{

  //Check time, update the SPIFFS JSON time
  // file, and save it before we sleep
  //updateFromJSON();
  
  //update LED SCREEN INCASE I NEED TIME WHILE ESP IS SLEEPING
  //updateHPDL();
  //SAVE POWER, OLED OFF!
  u8g2.clearBuffer();

  u8g2.drawStr(20,18,"ZZZZ...");
  u8g2.sendBuffer();
  //SLEEP FINALLY!            
  esp_sleep_enable_ext1_wakeup(BTN_BTMASKOLD,ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void IRAM_ATTR btnUP_INTRR()
{
  menuHandler(1);  
}

void IRAM_ATTR btnSL_INTRR()
{
  menuHandler(0);
}

void IRAM_ATTR btnDN_INTRR()
{
  menuHandler(2);  
}

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 
  pinMode(BTN_DN,INPUT);
  pinMode(BTN_UP,INPUT);
  pinMode(BTN_SL,INPUT);
  //attachInterrupt(BTN_DN, btnDN_INTRR, HIGH);
  //attachInterrupt(BTN_UP, btnUP_INTRR, HIGH);
 // attachInterrupt(BTN_SL, btnSL_INTRR, HIGH);
  
 ///Wire.begin(); 
 u8g2.begin();
 //updateFromJSON();

  /*
   //=====================
  //  32KhZ Crystal setup
  rtc_clk_32k_bootstrap(512);
  rtc_clk_32k_bootstrap(512);
  rtc_clk_32k_enable(true);
  uint32_t cal_32k = CALIBRATE_ONE(RTC_CAL_32K_XTAL);
  rtc_clk_slow_freq_set(RTC_SLOW_FREQ_32K_XTAL);
  //=====================
  //adc2_config_width(ADC_WIDTH_12Bit);
  // Battery Monitor
  adc2_config_channel_atten(ADC2_CHANNEL_9, ADC_ATTEN_0db); //set reference voltage to internal
  //ADC2_CH9 --> GPIO 26
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); 

 Wire.begin(); 
 u8g2.begin();
 sht20.begin();
 dopeScreen.begin();
 dopeScreen.printOverflow(true);
 updateFromJSON();
 checkSensors();
 updateHPDL(); 
 menuHandler(0);
   
*/
  //setupBLE();
 //tryWiFi_time();//see if we're in a known area for updates
  //espSleep();
    menuHandler(0);

Serial.println("all setup!");

}

void loop() {
  // put your main code here, to run repeatedly:

  menuHandler(0);
  delay(500);
  //updateFromJSON();
  //updateHPDL();   
  //espSleep();
}

//============================================================================
//                      MENU / UI INTERFACE
//============================================================================
void menuHandler(int dir)
{

//detach interrupts
detachInterrupt(BTN_UP);
detachInterrupt(BTN_SL);
detachInterrupt(BTN_DN);
int MAX_DIR=7;//move this up as more directories are added
            // ORRR...add another similar function to this with subdirs..
char temp[5];
int selection =0;
    struct tm * timeinfo;
     time_t t;
     timeval tval;
     gettimeofday(&tval, NULL);
     //convert to time values we can actually use
     t = tval.tv_sec;           //seconds since epoch
     timeinfo = localtime(&t);  //time struct
     int day=timeinfo->tm_mday;
     int month=timeinfo->tm_mon;
     month++;//no such thing as a 0th month

    u8g2.setFont(u8g2_font_tinytim_tf);

//ADD TIMER ALSO (2-3 SECONDS)
const long menuTimeout = 55;//about 5 seconds          
unsigned long counter = 0;      
bool hasSelection = false;

while (!hasSelection )
{
  
  u8g2.clearBuffer();
  if(digitalRead(BTN_UP)==HIGH)
    dir++;
  
  if(digitalRead(BTN_DN)==HIGH)
    dir--;
  if(dir >MAX_DIR) 
    dir=0;
  else if(dir <= -1)
    dir = MAX_DIR;
    
  switch(dir)
  {
    case 0:
    //OLED Default screen:
    //       T:XX*c H:XX%
    //D:DD M:MM     B:XX%
    
    dtostrf(temperature,5,2,temp);
    u8g2.drawStr(80,8,"T:");
    u8g2.drawStr(90,8,temp);
    
    u8g2.drawStr(80,16,"H:");
    dtostrf(humidity,5,2,temp);
    u8g2.drawStr(90,16,temp);

    dtostrf(battLevel,5,2,temp);
    u8g2.drawStr(90,24,temp);
    u8g2.drawStr(80,24,"B:");

    //units
    u8g2.drawStr(114,8,"C");
    u8g2.drawStr(114,16,"%");
    u8g2.drawStr(114,24,"%");
    //Month / Day  
    dtostrf(month,2,0,temp);
    u8g2.drawStr(2,22,temp);
    u8g2.drawStr(14,22,"/");
    dtostrf(day,2,0,temp);
    u8g2.drawStr(16,22,temp);
    selection =0;
     /*
     if(bleServerEnabled)
     {
      u8g2.drawStr(25,16,"BLE IS ON!");
      selection=255;
     }
     */
    if(isWebServing)
    {
        u8g2.clearBuffer();
        u8g2.drawStr(5,5,"WebServer Enabled!");
        u8g2.drawStr(5,15,"PW: 123456789");
        u8g2.drawStr(5,24,"-->>  192.168.4.1");
                            //192.168.4.1
        selection=255;
        
    }
    break;
case 1://scan wifi
        u8g2.drawStr(0,10,"Scan for known");
        u8g2.drawStr(20,18,"WiFi Networks");
        u8g2.drawStr(20,24,"and update time");
        selection=1;
    break;
case 2://open webServer
         u8g2.drawStr(5,10,"Start the WiFi Server");
         u8g2.drawStr(5,20,"To Load New Config!");       
         selection=2;
    break;
case 3: //IMU stuff 
          u8g2.drawStr(0,10,"IMU (currently unavailable)");
          selection =0;
   break;
case 4://games
        u8g2.drawStr(20,10,"Chrome Dino Game!");
          renderEnemy(1,80,20);
          selection =4;
   break;
case 5:
        u8g2.drawStr(20,10,"Space Invaders!");
        renderEnemy(5,80,20);
        selection =5;
   break;
case 6:
        u8g2.drawStr(5,15,"Stop the WiFi Server");
        //u8g2.drawStr(5,20,"this saves lots of power!");
        selection=6;
        break;
case 7:

       u8g2.drawStr(5,10,"Start BlueTooth Service");
       u8g2.drawStr(5,20,"Connect with Android App");
        selection=7;
        break;
default:
            u8g2.drawStr(5,10,"END OF MENU");
            u8g2.drawStr(20,18,"PRESS ^^ BUTTON");
    break; 
   }  
    u8g2.sendBuffer();
  //420 69
    counter++;
    hasSelection=digitalRead(BTN_SL);
    if(counter>menuTimeout && !isWebServing) //&& !bleServerEnabled
    {
        selection=0;
        hasSelection=true;
    }

    //delay is used for button debouncing
    //"fake" timer is used to exit menu and enter sleep mode
    delay(100);// transfer internal memory to the display
  
  
  
  }//iterate through whole menu until selection



switch(selection)
{
  case 0:
      //go to sleep??
      espSleep();
  break;
  case 1:
    tryWiFi_time();
  break;
  case 2:
    makeWiFiServer(); 
  break;  
  case 3:
    //IMU STUFF
  break;
  case 4:
      dinoGame();
  break;
  case 5:
      spaceInvaders();
  break;
  case 6:
      //stopBLE();
      stopWiFi();
      break;
  case 7:
      return;
    //setupBLE();
    //handleBle();
  break;
  
  default:
    return;
  break;
 }//end selection to function handler
  
//do something with dir --> open subdir or perform action
}//BTN SELECT


//============================================================================
//                      SENSORS
//============================================================================

void checkSensors()
{
    temperature=sht20.temperature();
    humidity=sht20.humidity();
    battLevel=battery_read();
}

static uint32_t calibrate_one(rtc_cal_sel_t cal_clk, const char *name)
{

    const uint32_t cal_count = 1000;
    const float factor = (1 << 19) * 1000.0f;
    uint32_t cali_val;
    printf("%s:\n", name);
    for (int i = 0; i < 5; ++i)
    {
        printf("calibrate (%d): ", i);
        cali_val = rtc_clk_cal(cal_clk, cal_count);
        printf("%.3f kHz\n", factor / (float)cali_val);
    }
    return cali_val;
}

float battery_read()
{
    //read battery voltage per %
    long sum = 0;                  // sum of samples taken
    float voltage = 0.0;           // calculated voltage
    float output = 0.0;            //output value
    const float battery_max = 3.6; //maximum voltage of battery
    const float battery_min = 2.9; //minimum voltage of battery before shutdown

    float R1 = 47000.0; // resistance of R1 (47K)
    float R2 = 47000.0;  // resistance of R2 (47K)
    int read_raw = 0;
    for (int i = 0; i < 100; i++)
    {
     // sum ADC2_CHANNEL_9
   esp_err_t r = adc2_get_raw( ADC2_CHANNEL_9, ADC_WIDTH_12Bit, &read_raw);
//        sum += adc1_get_voltage(ADC2_CH9);
        sum+=read_raw;   
        delayMicroseconds(1000);
    }
    // calculate the voltage
    voltage = sum / (float)500;
    //voltage = (voltage * 1.1) / 4096.0; //for internal 1.1v reference
    // use if added divider circuit
     voltage = voltage / (R2/(R1+R2));
    //round value by two precision
    voltage = roundf(voltage * 100) / 100;
    Serial.print("voltage: ");
    Serial.println(voltage, 2);
    output = ((voltage - battery_min) / (battery_max - battery_min)) * 100;
    if (output < 100)
        return output;
    else
        return 100.0f;
}



void updateHPDL()
{
  
 dopeScreen.clear();
 //Serial.println("======HPDL UPDATE======");

 struct tm * timeinfo;
 time_t t;
 timeval tval;
 gettimeofday(&tval, NULL);
  //convert to time values we can actually use
  t = tval.tv_sec;           //seconds since epoch
  timeinfo = localtime(&t);  //time struct
  int hour=timeinfo->tm_hour;
  int minute=timeinfo->tm_min;
  int isDaylightSavings=timeinfo->tm_isdst;

//because, murica!
  if(isDaylightSavings >0)
      hour=hour-1;

 //24 to 12 hour conversion  
   if(hour>12)
      hour-=12;

   if (hour==0)
       hour=12;
  
 String hourStr=String(abs(hour));
 String minuteStr = String(minute);
 //USE ALL 4 CHARACTERS ON HPDL
 if(hour < 10)
   hourStr="0"+hourStr;
 if (minute < 10)
    minuteStr ="0"+minuteStr;
    
 String timeNow = hourStr+minuteStr;
 dopeScreen.print(timeNow);  
 //Serial.print(timeNow);
 //Serial.println("\n======DONE!======");
}


//============================================================================
//                      WiFi HTTP SERVER
//============================================================================

AsyncCallbackJsonWebHandler* webTimeHandler = new AsyncCallbackJsonWebHandler("/updateTimeData", [](AsyncWebServerRequest *request, JsonVariant &json) {
  JsonObject newData = json.as<JsonObject>();
        
        int yr= newData["yr"];
        int mnt=newData["mnt"];
        int dy=newData["dy"];
        int hr=newData["hr"];
        int mint=newData["mint"];
        int sec=newData["sec"];
//  curl -i -d '{"yr":1,"mnt":2,"dy":3,"hr":4,"mint":5,"sec":6}'  -H "Content-Type: application/json"  -X POST http://192.168.4.1/updateTimeData
// curl -i  -X GET http://192.168.4.1/getTime   
        
        didWebUpdate = updateJsonTime(yr,mnt,dy,hr,mint,sec);
        updateSysTime(yr,mnt,dy,hr,mint,sec);
       
       String respData;
       
       serializeJson(newData,respData);
       request->send(200, "application/json", respData);
       
        if(didWebUpdate)
          request->send(200, "application/json", respData);
        else
            request->send(400, "application/json", respData);
      });


AsyncCallbackJsonWebHandler* webConfHandler = new AsyncCallbackJsonWebHandler("/addConf", [](AsyncWebServerRequest *request, JsonVariant &json)
{
  JsonObject conf2Add = json.as<JsonObject>();
  const char* newW = conf2Add["W"];
  const char* newP = conf2Add["P"];

  didWebUpdate = updateJsonConfig(newW,newP);

  if(didWebUpdate)
    request->send(200, "text/plain", newW );
  else
    request->send(400,"text/plain", newW);  
});

void webGetSpiffsTime(AsyncWebServerRequest *request) {
  String buff;
  if(!SPIFFS.begin(true)){
    buff= "SPIFFS ERR";
  }
  File rawFile = SPIFFS.open("/sysTime.json","r");
  
  if(!rawFile){
         buff= "FILE ERR";
        }
  while(rawFile.available())
  {
    buff+=char(rawFile.read()); 
  }  
  rawFile.close();
  request->send(200, "application/json", buff );
}


void webGetSpiffsConf(AsyncWebServerRequest *request) {
  String buff;
  if(!SPIFFS.begin(true)){
    buff= "SPIFFS ERR";
  }
  File rawFile = SPIFFS.open("/userConf.json","r");
  
  if(!rawFile)
  {
     buff= "FILE ERR";
  }
  while(rawFile.available())
  {
    buff+=char(rawFile.read()); 
  }  
  rawFile.close();
  request->send(200, "application/json", buff );
}

void webGetSensrData(AsyncWebServerRequest *request)
{
   checkSensors();
   String buff;
   StaticJsonDocument<200> dataOut;
    dataOut["B"] = battLevel;
    dataOut["T"] = temperature;
    dataOut["H"] = humidity;
    
  serializeJson(dataOut,buff);
  request->send(200, "application/json", buff );
}

void makeWiFiServer()
{
  isWebServing = true;
  WiFi.softAP("CopperHead TimePiece","123456789");
  //192.168.4.1
/* //http:myWatch.local
if(!MDNS.begin("myWatch")) {
     return;
}
*/
  server.on("/",[](AsyncWebServerRequest *request){
    
        if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
         }
    request->send(SPIFFS, "/index.html", "text/html");
  });
 
  // attach request handler
  server.addHandler(webTimeHandler);
  server.addHandler(webConfHandler);
  server.on("/getTime",webGetSpiffsTime);
  server.on("/getConf",webGetSpiffsConf);
  server.on("/getSensorData",webGetSensrData);
  server.begin();
  isWebServing = true;
return;
}


//============================================================================
//        DINOSAUR CHROME GAME (404-No Internet)
//============================================================================

const unsigned char dinoHexTiny[] PROGMEM = {
    0xe0, 0xb0, 0xf8, 0x2c, 0xf6, 0xbf, 0x24, 0x6c
};
const unsigned char birdHexTiny[] PROGMEM = {
  0x78, 0x18, 0x88, 0xfe, 0xff, 0x88, 0x18, 0x78

};
const unsigned char tinyCactusHex[] PROGMEM = {
  0x08,0x9D,0xB5,0xFF,0x16,0x1C,0x14,0x1C
};

struct player{
  int health;// not needed for dino game, possibly something else
  int x;
  int y;
  boolean isAlive;
  };
struct enemyEntity{
  bool render;
  int type;//1 cactus 2 is bird
  int x;
  int y;
  };
enemyEntity newEnemy();

void dinoGame()
{
 int score=0;
 bool isJumping = false;
 player p  = {100,14,FLOOR_LEVEL,true};

 enemyEntity enemies[12]; 
  for(int i=0;i<12;i++)
  {   
    int rX = ENEMY_START+(ENEMY_PAD*i);
    int y=FLOOR_LEVEL;///cactus
    enemies[i] = {true,1,rX,y};
  }
  
  bool isJump = false; 
  int level=2;
  int enemySpeed=2;
  
 char buff[5];
 while(p.isAlive)
 {
//clear screen
  u8g2.clearBuffer();
  isJumping=digitalRead(BTN_SL);
// check button for jump
  if(isJumping && (p.y>5 ))
      p.y -=JUMP_PIXELS;
  // render Game Data  
  dtostrf(score,5,2,buff);
  u8g2.setFont(u8g2_font_tinytim_tf);
  u8g2.drawStr(65,10,"SCORE:");
  u8g2.drawStr(100,10,buff);

  u8g2.drawStr(65,20,"LV:");
  dtostrf(level,5,2,buff);
  u8g2.drawStr(100,20,buff);


  //process enemies (1 loop to rule them all!)
   for(int i=0;i<12;i++)
    {
      if(checkCollision(p.x,p.y,enemies[i].x,enemies[i].y))
          p.isAlive=false;
      if(enemies[i].render)
        renderEnemy(enemies[i].type,enemies[i].x,enemies[i].y);
      else{
        int rX = random(ENEMY_START,WIDTH+50)+ENEMY_PAD;
        int rType = 1;
        int y=FLOOR_LEVEL;//CACTUS y
        
            if(level>5)
            {
             rType = random(0,3);
             if(rType>1)
             {
              rType=2;
              y=random(IMG_SIZE-1,FLOOR_LEVEL);
             }
            }
        enemyEntity newE = {true,rType,rX,y};
        enemies[i] = newE;
        }   
        //MOVE ALL ENEMIES LEFT!
        enemies[i].x-=enemySpeed;
        //ERASE ENEMIES AFTER PLAYER.X
        if(enemies[i].x < 0)
          enemies[i].render = false;
    }

 //gravity!
 if(p.y<=FLOOR_LEVEL)
      p.y++;
 renderPlayer(p.x,p.y,1);  
 
 
 u8g2.sendBuffer(); 
  
  score++;
  if(score%100==0)
    level++;
    
  else if(score%700==0)
  {
    score+=1000;
    enemySpeed+=2;
    level+=2;
  }
  delay(100);
  
  }//end ALIVE
  u8g2.clearBuffer();
  
u8g2.setFont(u8g2_font_tinytim_tf);
dtostrf(score,5,2,buff);
u8g2.drawStr(65,10,"SCORE:");
u8g2.drawStr(100,10,buff);
u8g2.drawStr(65,20,"LV:");
dtostrf(level,5,2,buff);
u8g2.drawStr(100,20,buff);
u8g2.drawStr(10,10,"YOU DED");
u8g2.drawStr(20,30,"X(");
renderPlayer(40,24,1); 
u8g2.sendBuffer(); 

// get EEPROM high score data
// compare to current score
//save if needed
//go back to application menu 

while (! digitalRead(BTN_SL)==HIGH){}
}


//============================================================================
//        SPACE INVADERS (3 BUTTONS)
//============================================================================
const unsigned char evilCloud [] PROGMEM = {
  0x08, 0x7e, 0x93, 0x91, 0xff, 0xa5, 0x8b, 0xff
};

const unsigned char evilAlien1[] PROGMEM={  
  0x3c, 0x5a, 0xff, 0xbd, 0x18, 0x24, 0x42, 0x81
};

const unsigned char evilAlienOriginal [] PROGMEM = {
  0x42, 0x3c, 0x7e, 0xdb, 0xdb, 0xff, 0x42, 0x66
};
const unsigned char evilSquidHead [] PROGMEM = {
  0x18, 0x3c, 0x7e, 0xc9, 0xdb, 0x7e, 0x55, 0x55
};


struct bulletEntity{
  int x;
  int y;
  bool renderable;
  };


void spaceInvaders()
{
  
 //init game and ALL enemies 
 int loopCount=0;//counter to stop enemies from rushing down too quick
 int enemySpeed=15;
 int level=1;
  int score=0;
  int bulletCount =0;
  player p  = {100,14,FLOOR_LEVEL,true};
  int enemy_count = 16;
  enemyEntity enemies[4][4];
  bulletEntity bullets[5];
  char buff[5];

  for(int i=0;i<4;i++)
  {   
    int rX = i*24;
    for(int j=0;j<4;j++)
    {
        int y =(j*-16)+i;
        enemies[i][j] = {true,4,rX,y};
    }
  }
   while(p.isAlive)
  {
    u8g2.clearBuffer();
    
    
  dtostrf(enemy_count,5,2,buff);
  u8g2.setFont(u8g2_font_tinytim_tf);
  u8g2.drawStr(90,10,"#:");
  u8g2.drawStr(100,10,buff);
  u8g2.drawStr(75,20,"WAVE:");
  dtostrf(level,5,2,buff);
  u8g2.drawStr(100,20,buff);

    //USER CONTROLS
    bool makeBullet=false;
  if(digitalRead(BTN_UP) ==HIGH)
    {
      p.x-=2;
      if(p.x<4)
        p.x=4;
    }
  else if(digitalRead(BTN_DN)==HIGH)
    {
      p.x+=2;
      if(p.x>124)
        p.x=124;//8 pixels before SCREEN_WIDTH
    }
    
  if((digitalRead(BTN_SL)==HIGH)&&( bulletCount <2))
    {
      Serial.println(bulletCount);
      
        bulletEntity newBullet = {p.x,p.y+3,true};
        bullets[bulletCount]=newBullet;
        bulletCount++;  
    }
    
    
  for(int i=0;i<4;i++)
  {//going in...hopefully most can be skipped
    for(int j=0;j<4;j++)
    { //oof loop 2

      //ONLY DO ALL OF THIS IF AN ENEMY IS ALIVE
      if( enemies[i][j].render)
      {
      //DO ALL BULLETS FOR EACH ENEMY X(
     // SORRY LIL COMPUTER, GAMES ARE HARD...but more likely my unoptimized code
       for(int b=0;b<bulletCount;b++)
       {
          if((bullets[b].renderable) && (checkCollision(bullets[b].x,bullets[b].y,enemies[i][j].x,enemies[i][j].y)==true))
          {
            enemies[i][j].render=false;//HIT, mark for deletion   
            bullets[b].renderable=false;
            bulletCount--;
            enemy_count--;
          }
             
          if(bullets[b].renderable)
          {
            renderBullet(bullets[b].x,bullets[b].y,1);
            bullets[b].y--;
            //Serial.println(bullets[b].y);
          }
          if(bullets[b].y<0)
          {
            bullets[b].renderable=false;
            bulletCount--;  
          }
      }
      renderEnemy(enemies[i][j].type,enemies[i][j].x,enemies[i][j].y);
      
      if(loopCount%enemySpeed==0)
          enemies[i][j].y++;  
      if(enemies[i][j].y >= FLOOR_LEVEL)
      {
        p.isAlive = false;//THEY LANDED! we stood no chance..
        enemies[i][j].render=false;
        enemy_count--;
      }
    }//end RENDERABLE ENEMY PROCESSING
    }//END j LOOP
  }//END i LOOP  
  
//check collision between renderable enemies and PLAYER_WIDTH

renderPlayer(p.x,p.y,2);
u8g2.sendBuffer();
loopCount++;

if(enemy_count<=0)
{
   int type = random(3,7);
   for(int i=0;i<4;i++)
   {   
    int rX = i*random(0,30)+8;
    for(int j=0;j<4;j++)
    {
      int y;
      if(level<4)
         y =(j*-16)+i;
      else
          y=(j*-(16-random(1,level)))+i;  
        enemies[i][j] = {true,type,rX,y};
    }
  }
  enemy_count = 16;  
  enemySpeed-=5;
  if (enemySpeed <=0)
        enemySpeed =2;
  level++;
  loopCount=0;
}

delay(10);//button debounce

 }//end of players life

 //
  u8g2.clearBuffer(); 
  dtostrf(enemy_count,5,2,buff);
  u8g2.setFont(u8g2_font_tinytim_tf);

 // u8g.drawStr(10,20,"LAND");
  u8g2.drawStr(10,5,"THE WORLD HAS ENDED X(");

  
  u8g2.drawStr(40,15,"ALIENS LEFT: ");
  u8g2.drawStr(100,15,buff);
  u8g2.drawStr(65,25,"WAVE:");
  dtostrf(level,5,2,buff);
  u8g2.drawStr(100,25,buff);

  
  u8g2.sendBuffer();

  while (! digitalRead(BTN_SL)==HIGH){}

 }


 
void renderBullet(int x,int y, int r)
{
 u8g2.drawCircle(x,y,r,U8G2_DRAW_ALL);  
}

//============================================================================
//        MULTI GAME FUNCTIONS
//============================================================================
void renderEnemy(int type,int x,int y)
{
  if(type ==1)
        u8g2.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, tinyCactusHex); 
  else if (type ==2)
        u8g2.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, birdHexTiny); 
  else if (type ==3)
        u8g2.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, evilCloud); 
  else if (type ==4)
        u8g2.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, evilAlien1); 
  else if (type ==5)
        u8g2.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, evilAlienOriginal); 
  else if (type ==6)
        u8g2.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, evilSquidHead);       
       
}
void renderPlayer(int x,int y,int game)
{ 
  if (game ==1)
      u8g2.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, dinoHexTiny); 
  else if (game ==2)
      u8g2.drawBox(x,y,4,4);
}

boolean checkCollision(int x1,int y1,int x2,int y2)
{ 
  if(x1 < x2 +IMG_WIDTH && x1+IMG_WIDTH > x2  &&
     y1 < y2+IMG_HEIGHT && y1+IMG_HEIGHT >y2)
     {
      return true; 
     }
    else
      return false;
}
/*
// IMPLEMENTATION PENDING ANDROID APP DEVELOPMENT
// CURRENT WORKAROUND--> CREATE / HOST WEBSERVER
// BLE GLOBALS
#define USERCONF_UUID BLEUUID((uint16_t) 0x181C)//user Data 
#define CHAR_UUID "982795fa-f812-44d0-9a31-11431883fb04" 

#define timeSERVICE_UUID  BLEUUID((uint16_t) 0x2A08)
#define timeCharUUID "b0b354c6-d81f-11eb-b8bc-0242ac130003"

//#define tempServiceUUID BLEUUID((uint16_t) 0x2A1C)
//#define tempCharUUID "b0b350b6-d81f-11eb-b8bc-0242ac130003"

#define batteryServiceUUID BLEUUID((uint16_t) 0x180F)
#define batteryCharUUID "b0b353fe-d81f-11eb-b8bc-0242ac130003"

OTHER 
b0b3530e-d81f-11eb-b8bc-0242ac130003
b0b353fe-d81f-11eb-b8bc-0242ac130003
b0b354c6-d81f-11eb-b8bc-0242ac130003
bool bleDeviceConnected = false;
bool oldBleDevice = false;
bool bleServerEnabled = false;
bool didBleUpdate = false;
BLEServer *pServer = NULL;
 
class bleServerCallbacks: public BLEServerCallbacks{
  
   void onConnect(BLEServer *pServer)
   {
     bleDeviceConnected = true;
   }
   void onDisconnect(BLEServer *pServer)
   {
     bleDeviceConnected = false;
   }
  
  };

//BLE characteristic Handling
class BLEuserConfCallbacks: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pChar)
  {
    std::string value = pChar->getValue();
    if(value.length ()>0)
    {
      //ADD JSON INPUT HERE
     // for(int i=0;i<value.length();i++)
      //{
        //bleDataIn[i]=value[i]; 
      //}
      //bleDataIn[value.length()]='\0';
      StaticJsonDocument<256> newData;
      DeserializationError err = deserializeJson(newData,value.c_str());
      if(err)
        return;
        //Serial.println("JSON CONFIG OPEN ERR");
       const char* updateSSID = newData["W"];
       const char* newPWD  = newData["P"];
       didBleUpdate =  updateJsonConfig(updateSSID,newPWD);
      //INPUT TIME STRING WITH ABILITY TO DECONSTRUCT:
    }    
  }
  
  void onRead(BLECharacteristic *pChar)
  {
   //return data to phone (send OKS or ERRS) 
  }
};


class BLEtimeUpdtCharCallbacks: public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pChar)
  {
    std::string value = pChar->getValue();
    if(value.length ()>0)
    {
      //ADD JSON INPUT HERE
     // for(int i=0;i<value.length();i++)
      //{
        //bleDataIn[i]=value[i]; 
      //}
      //bleDataIn[value.length()]='\0';
      StaticJsonDocument<256> newData;
      DeserializationError err = deserializeJson(newData,value.c_str());
      if(err)
        return;
        //Serial.println("JSON CONFIG OPEN ERR");
        
      //INPUT TIME STRING WITH ABILITY TO DECONSTRUCT:
      int yr= newData["yr"];
      int mnt=newData["mnt"];
      int dy=newData["dy"];
      int hr=newData["hr"];
      int mint=newData["mint"];
      int sec=newData["sec"];
//    updateSysTime(yr,mnt,dy,hr,mint,sec);
      didBleUpdate = updateJsonTime(yr,mnt,dy,hr,mint,sec);
      //======
    }  
      /*
        u8g2.clearBuffer();
        u8g2.drawStr(5,10,"BLE data!");
        u8g2.drawStr(1,18,value.c_str()); 
        u8g2.sendBuffer();
        delay(5000);
      * /
  
  }

    void onRead(BLECharacteristic *pChar)
  {
   //return data to phone (send OKS or ERRS) 
  }
  
};


void setupBLE()
{
  
  stopWiFi();//make sure we have the antenna!
  
  BLEDevice::init("CopperHead Timepiece");
  pServer = BLEDevice ::createServer();

  pServer->setCallbacks(new bleServerCallbacks());
    
  BLEService *pService = pServer->createService(USERCONF_UUID);

  BLEService *timeService = pServer->createService(timeSERVICE_UUID);
  
  BLECharacteristic *timeChar= timeService->createCharacteristic(timeCharUUID, 
            BLECharacteristic::PROPERTY_READ | 
            BLECharacteristic::PROPERTY_WRITE);

  BLECharacteristic *pSecureUsrCfChar = pService->createCharacteristic(
    CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    
    pSecureUsrCfChar->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pSecureUsrCfChar->setCallbacks(new BLEuserConfCallbacks());
    pSecureUsrCfChar->setValue("Hello Watch!");

    timeChar->setValue("fuck!");
    timeService->start();
    pService->start();
    
    BLEAdvertising *pAdvert = pServer->getAdvertising();
    pAdvert->start();

    bleServerEnabled=true;    
        u8g2.clearBuffer();
        u8g2.drawStr(5,10,"BLE Started!");
        u8g2.drawStr(5,18,"Go Connect!");
        u8g2.sendBuffer();
        delay(5000);
}

void stopBLE()
{
        BLEDevice::deinit(true);
        u8g2.clearBuffer();
        u8g2.drawStr(5,10,"Turning BLE");
        u8g2.drawStr(5,20,"OFF");
        u8g2.sendBuffer();
        bleServerEnabled=false;
        delay(3000);
}


void handleBle()
{
  //
  if(bleDeviceConnected)
  {
        u8g2.clearBuffer();
        u8g2.drawStr(5,10,"Device Connected!");
       // u8g2.drawStr(5,18,"Something");
       // u8g2.drawStr(5,24,"find a known network! X(");
        u8g2.sendBuffer();
        delay(5000);
  }
//disconnecting
  if(!bleDeviceConnected && oldBleDevice)
  {
    //end all service advertising
    oldBleDevice = bleDeviceConnected;
   //stopBLE();
        u8g2.clearBuffer();
        u8g2.drawStr(5,10,"Device Disconnecting!");
        u8g2.sendBuffer();
        delay(4000);
   }
  //connecting
  if(bleDeviceConnected && !oldBleDevice)
  {
    //do something on connection
    oldBleDevice = bleDeviceConnected;
       u8g2.clearBuffer();
       u8g2.drawStr(5,10,"Device Connecting!");
       u8g2.sendBuffer();
       delay(2000);
  }
}
*/
