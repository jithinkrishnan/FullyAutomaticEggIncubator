/************************************************************************
*   Arduino EGG Incubator [Original code Modified]
*   
*   File:   Egg_Incubator.ino
*   Author:  Jithin Krishnan.K
*       Rev. 1.0.0 : 10/12/2018 :  10:55 PM
* 
* This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* https://forum.arduino.cc/index.php?topic=491829.0
* https://www.backyardchickens.com/threads/arduino-incubator-controls-for-dummies.1189629/
*
* Email: jithinkrishnan.k@gmail.com
*   
************************************************************************/
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);
#define DHT1 2
#define DHTTYPE DHT11   // DHT 22  (AM2302)
#define humidityRelay 10 //relays
#define alarmrelay A3 //relays
#define turnerrelay1 A1 //relays
#define turnerrelay2 A2 //relays
#define overrun 9
#define turnerinput 8 //relays
#define heaterRelay A0 //heater relay
#define lightrelay 12
#define ON  HIGH
#define OFF LOW

DHT sensor1(DHT1, DHTTYPE);
const byte buttonEdit = 5;
const byte buttonPlus = 4;
const byte buttonMinus = 7;
const byte buttonEnter = 6;//make into lights


char* menu1[8] = {"set temp/humidity", "set time/date", "set turner",
                  "set hatch date", "alarm points", "hardware setup", "factory reset", "return"
                 };
char* menu10[9] = {"current set points", "current cal offset", "set point temp", "set point humidity", "calibrate temp",
                   "calibrate hum", "set temp swing", "set humidity swing", "return"
                  };
char* menu20[6] = {"set minute", "set hour", "set day",
                   "set month", "set year", "return"
                  };
char* menu30[3] = { "disable/enable", "return", " "
                  };

char* menu40[4] = {"set days to hatch", "start countdown", "alarm on/off",
                   "return"
                  };
char* menu50[5] = {"turner hardware", "temp in F or C",
                   "audio alarm", "hot spot probe", "return"
                  };
char* menu60[3] = {"reset now ", "return", " "};
char* menu80[8] = {"temp alarm", "humidity alarm", "display set points", "high temp sp",
                   "low temp sp", "high hum sp", "low hum alarm", "return"
                  };

char* menu100[5] = {"disable/enable", "time turner runs", "time between turns",
                    "test turner", "return"
                   };
char* menu110[5] = {"disable/enable", "time between turns",
                    "cam time", "test turner", "return"
                   };
char* menu120[6] = {"disable/enable", "time between turns", "turner manual",
                    "tuner alarm", "test turner", "return"
                   };

char* standardText[4] = {"use up and down", "press enter to apply", "set_point : ", "press enter to exit"};
int subsublevel = 0;
int menuNumber2 = 0;
float temp;
float humidity;
float humidity_setpoint = 0;
float temp_setpoint = 0;
int alarm_active = 0;
float tempC = 0;
float tempF = 0;
float highTempSetPoint = 0;
float lowTempAlarmPoint = 0;
float highHumidAlarmPoint = 0;
float lowHumidAlarmPoint = 0;
unsigned long secondTurnercounter = 0; // turner second timer rewite to 60 then reset
unsigned long previousMillis5 = 0;
unsigned long timeBetweenTurnsSeconds = 0;
long previousMillis = 0;//one second counter
long interval = 1000; //one second
float tempSwing = 0;
float humiditySwing = 0;
float cal_temp = 0;
float cal_humidity = 0;
float adj_temp;
float adj_humidity;
long turn_timer; // used on display
int display_stat_turn = 1;
int t1_hatch_days = 15;
int time_to_turn_mins;
int time_to_turn_hr;
int set_minute = 1; //used to set rtc
int set_hour = 1; //used to set rtc
int set_day = 1; //used to set rtc
int set_month = 1; //used to set rtc
long set_year = 2018; //used to set rtc
int tunerDisable = 0;
int clock_update = 0; //used to set rtc
byte reboot = 1;
int prev_clock_day = 0;
int hour_counter = 0;
int memory_update;
unsigned int temp_timer = 0; // stop sensor for one second on boot and slow reads
byte lightButton = 0;
byte editButton = 0;
byte plusButton = 0;
byte noSave = 0;
byte  minusButton = 0;
int  menuNumberLv1 = 0;
int menuNumber3 = 0;
int editMode = 0;
byte preveditButton = 0;
int menuNumber = 1;
int subMenu = 0;
byte prevplusButton = 0;
byte prevMinusButton = 0;
unsigned long prevMillis = 0;
byte prevlightButton = 0;
int prevmn = 0;
int screenmax = 7;
int menuLevel = 0;
int prevmn2 = 0;
int escape = 0;
int hatchDays = 0;
int hatchCountUp = 0;
int hatchAlarm = 0;
int turnerType = 1;
int TimeBetweenTurns = 1;
int TimeTurnerTurns = 1;
int turnerAlarm = 0;
unsigned long  turnerRunCounter = 0;
int testTurner = 0;
int countdown = 0;
byte tilt = 0;
byte onetime = 0;
byte turnerstop = 0;
byte turnerEstop = 0;
byte prevturnerstop = 0;
byte audioalarm = 0;
byte tempalarm = 1;
byte humalarm = 1;
byte facReset = 0;
byte soundalarm = 0;
byte badsensor = 1; //bad reading go safe
byte setlightrelay = 1;
byte heaterdisplay = 0;
byte humidityfandisplay = 0;
byte tempinF = 0;
byte prevtempinF = 0;
byte temp_char[8] = {B01110, B01010, B01010, B01110, B01110, B11111, B11111, B01110};
byte hum_char[8] = {B00100, B01110, B11111, B00100, B10001, B00100, B10001, B00100};
byte bell_Char[8] = {B00100, B01110, B01110, B01110, B11111, B11111, B00100, B00000};
byte arrow_Char[8] = {B00000, B00000, B10000, B10000, B10111, B10011, B10101, B01000};
byte pointer[8] = {B10000, B01000, B00100, B00010, B00010, B00100, B01000, B10000};
byte showC[8] = {B11000, B11000, B00000, B00111, B00100, B00100, B00100, B00111};
byte showF[8] = {B11000, B11000, B00000, B00111, B00100, B00111, B00100, B00100};

//DS1307 clock;
DS3231 clock;
RTCDateTime dt;
sensor_t sensor;

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  //lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, temp_char);
  lcd.createChar(2, hum_char);
  lcd.createChar(3, bell_Char);
  lcd.createChar(4, arrow_Char);
  lcd.createChar(5, pointer);
  lcd.createChar(6, showF);
  lcd.createChar(7, showC);

  pinMode (buttonEdit, INPUT_PULLUP);
  pinMode (buttonPlus, INPUT_PULLUP);
  pinMode (buttonMinus, INPUT_PULLUP);
  pinMode (buttonEnter, INPUT_PULLUP);
  pinMode (turnerinput, INPUT_PULLUP);
  pinMode (overrun, INPUT_PULLUP);
  pinMode (humidityRelay, OUTPUT);
  pinMode (alarmrelay, OUTPUT);
  pinMode (turnerrelay1, OUTPUT);
  pinMode (turnerrelay2, OUTPUT);
  pinMode (heaterRelay, OUTPUT);
  pinMode (lightrelay, OUTPUT);
  clock.begin();
  sensor1.begin();
  lcd.clear();

  EEPROM.get (0, temp_setpoint); //float
  turnerAlarm = EEPROM.read (4); //int
  EEPROM.get (8,  highTempSetPoint ); //float
  EEPROM.get (12,  lowTempAlarmPoint ); //float
  EEPROM.get (16,  highHumidAlarmPoint ); //float
  EEPROM.get (20,  lowHumidAlarmPoint ); //float
  audioalarm = EEPROM.read (24); //int
  tempalarm = EEPROM.read (28); //int
  humalarm = EEPROM.read (32); //int
  hatchAlarm = EEPROM.read (36); //int
  tempinF = EEPROM.read (40); //int
  EEPROM.get (48, cal_temp ); //float
  EEPROM.get (52, cal_humidity); //float
  EEPROM.get (56, tempSwing); //float
  EEPROM.get (60, humiditySwing); //float
  hatchDays = EEPROM.read (64); //int
  hatchCountUp = EEPROM.read (68); //int
  turnerType = EEPROM.read (72); //int
  tunerDisable = EEPROM.read (76); //int
  TimeBetweenTurns = EEPROM.read (80); //int
  TimeTurnerTurns = EEPROM.read (84); //int
  EEPROM.get (88, humidity_setpoint);//float

}



//...................................................................................loop
void loop() {
  clockset();
  memoryUpdater();
  getTemp();
  humidityFan();
  screenCall();
  checkeditbutton();
  turnerCountForDisplay();
  alarmcheck();
  turnercontrols();
  //showprint();//used in testing
  heatercontrols();
  timers();
}
//......................................................................................loopend


// testMenu(menuNumber, screenmax, menuLevel);
void testMenu(int x, int y, int z ) {
  int a;
   lcd.setCursor(6, 0);
  // lcd.print(x);//used for testing
  //lcd.setCursor(4, 0);//used for testing
  // lcd.print(y);//used for testing
  // lcd.setCursor(8, 0);//used for testing
  // lcd.print(z);//used for testing
  lcd.print (F("menu"));
  int b;
  if (x == 1) {
    lcd.setCursor(0, 1);
    a = x - 1;
  }
  else if ((x == y) && (y != 2)) {
    lcd.setCursor(0, 3);
    a = x - 3;
  }
  else {
    lcd.setCursor(0, 2);
    a = x - 2;
  }
  lcd.write(5);
  for (int i = 1; i <= 3; i++) {
    lcd.setCursor(2, i);

    switch (z) {
      case 1:
        lcd.print( menu1[a]);
        break;
      case 10:
        lcd.print( menu10[a]);
        break;
      case 20:
        lcd.print( menu20[a]);
        break;
      case 30:
        lcd.print( menu30[a]);
        break;
      case 40:
        lcd.print( menu40[a]);
        break;
      case 50:
        lcd.print( menu50[a]);
        break;
      case 60:
        lcd.print( menu60[a]);
        break;
      case 80:
        lcd.print( menu80[a]);
        break;
      case 100:
        lcd.print( menu100[a]);
        break;
      case 110:
        lcd.print( menu110[a]);
        break;
      case 120:
        lcd.print( menu120[a]);
        break;
      default:
        lcd.print("error ");
        lcd.print(z);
        break;
    }
    a = a + 1;
    delay(10);
  }
}
void screenCall() {
  if (prevmn != menuNumber) {
    lcd.clear();
  }
  prevmn = menuNumber;
  if (prevmn2 != menuNumber2) {
    lcd.clear();
  }
  prevmn2 = menuNumber2;
  switch (editMode) {
    case 0:
      callHomeScreen();
      break;
    case 1:
      buttons(1);
      menuNumber = constrain(menuNumber, 1, 8);
      screenmax = 8;
      menuLevel = 1;
      testMenu(menuNumber, screenmax, menuLevel);
      subMenu = menuNumber;
      menuNumber2 = 0;
      break;
    case 2:
      buttons(2);
      menuNumber = 1;
      menuNumber2 = constrain(menuNumber2, 1, screenmax);
      switch (subMenu) {
        case 1:
          screenmax = 9;
          menuLevel = 10;
          testMenu(menuNumber2, screenmax, menuLevel);
          break;
        case 2:
          screenmax = 6;
          menuLevel = 20;
          testMenu(menuNumber2, screenmax, menuLevel);
          break;
        case 3:
          switch (turnerType) {
            case 1:
              screenmax = 2;
              menuLevel = 30;
              testMenu(menuNumber2, screenmax, menuLevel);
              break;
            case 2:
              screenmax = 5;
              menuLevel = 100;
              testMenu(menuNumber2, screenmax, menuLevel);
              break;
            case 3:
              screenmax = 5;
              menuLevel = 110;
              testMenu(menuNumber2, screenmax, menuLevel);
              break;
            case 4:
              screenmax = 6;
              menuLevel = 120;
              testMenu(menuNumber2, screenmax, menuLevel);
              break;
          }
          break;
        case 4:
          screenmax = 4;
          menuLevel = 40;
          testMenu(menuNumber2, screenmax, menuLevel);
          break;
        case 5:
          screenmax = 8;
          menuLevel = 80;
          testMenu(menuNumber2, screenmax, menuLevel);
          break;
        case 6:
          screenmax = 5;
          menuLevel = 50;
          testMenu(menuNumber2, screenmax, menuLevel);
          break;
        case 7:
          screenmax = 2;
          menuLevel = 60;
          testMenu(menuNumber2, screenmax, menuLevel);
          break;
        case 8:
          editMode = editMode - 2;
          break;
      }
      subsublevel = menuLevel + menuNumber2;
      break;
    case 3:
      buttons(3);
      menuNumber3 = constrain(menuNumber3, 1, screenmax);
      delay(20);
      escape = 0;
      switch (subsublevel) {

        case 11:
          //  display set points temp/humidity

          while (escape == 0) {
            checkeditbutton();
            lcd.setCursor(4, 0);
            lcd.print(F("set points"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[2]);
            lcd.write(1);
            addSpace(1);
            lcd.print (temp_setpoint, 1);
            addSign();
            lcd.setCursor(0, 2);
            lcd.print(standardText[2]);
            lcd.write(2);
            addSpace(1);
            lcd.print (humidity_setpoint, 0);
            lcd.setCursor(0, 3);
            lcd.print(standardText[3]);
          }
          editMode = editMode - 1;
          break;
        case 12:
          //display off sets
          while (escape == 0) {
            checkeditbutton();
            lcd.setCursor(1, 0);
            lcd.print(F("senor"));
            lcd.setCursor(11, 0);
            lcd.print(F("adjusted"));
            lcd.setCursor(0, 1);
            lcd.print("s");
            addSpace(1);
            lcd.write(1);
            lcd.print (temp, 1);
            addSign();
            lcd.setCursor(10, 1);
            lcd.print(F("="));
            addSpace(1);
            lcd.write(1);
            lcd.print (adj_temp, 1);
            addSign();
            lcd.setCursor(0, 2);
            lcd.print("s");
            addSpace(1);
            lcd.write(2);
            lcd.print (humidity, 1);
            lcd.setCursor(10, 2);
            lcd.print(F("="));
            addSpace(1);
            lcd.write(2);
            lcd.print (adj_humidity, 1);
            lcd.setCursor(0, 3);
            lcd.print(standardText[3]);
          }
          editMode = editMode - 1;
          break;
        case 13:
          //set temp here
          while (escape == 0) {
            checkeditbutton();
            buttons(10);
            lcd.setCursor(4, 0);
            lcd.print (F("change temp"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[2]);
            lcd.print (temp_setpoint, 1);
            addSign();
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 14:
          //set humidity here

          while (escape == 0) {
            checkeditbutton();
            buttons(11);
            lcd.setCursor(0, 0);
            lcd.print (F("change humidity"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[2]);
            lcd.print (humidity_setpoint, 0);

            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 15:
          //cal temp here
          while (escape == 0) {
            checkeditbutton();
            buttons(12);
            getTemp();
            lcd.setCursor(0, 0);
            lcd.print (F("current temp"));
            addSpace(1);
            lcd.print (temp, 1);
            addSign();
            lcd.setCursor(0, 1);
            lcd.print (F("cal offset:"));
            addSpace(1);
            lcd.print (cal_temp, 1);
            addSign();
            lcd.setCursor(0, 2);
            lcd.print (F("adjust temp"));
            addSpace(1);
            adj_temp = temp - cal_temp;
            lcd.print (adj_temp, 1);
            addSign();
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 16:
          //cal humidity here
          while (escape == 0) {
            checkeditbutton();
            buttons(13);
            getTemp();
            lcd.setCursor(0, 0);
            lcd.print (F("current "));
            lcd.write(2);
            addSpace(1);
            lcd.print (humidity, 1);
            lcd.setCursor(0, 1);
            lcd.print (F("cal offset:"));
            addSpace(1);
            lcd.print (cal_humidity, 1);
            lcd.setCursor(0, 2);
            lcd.print (F("adjust"));
            addSpace(1);
            lcd.write(2);
            addSpace(1);
            adj_humidity = humidity - cal_humidity;
            lcd.print (adj_humidity, 1);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 17:
          //set temp swing here
          while (escape == 0) {
            checkeditbutton();
            buttons(14);
            getTemp();
            lcd.setCursor(0, 0);
            lcd.print (F("temp drop before"));
            lcd.setCursor(0, 1);
            lcd.print (F("heater starts "));
            lcd.setCursor(0, 2);
            lcd.print (F("adjust"));
            addSpace(1);
            lcd.write(1);
            addSpace(1);
            lcd.print (tempSwing);
            addSign();
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 18:
          //set cal swing here
          while (escape == 0) {
            checkeditbutton();
            buttons(15);
            getTemp();
            lcd.setCursor(0, 0);
            lcd.print (F("hum drop before"));
            lcd.setCursor(0, 1);
            lcd.print (F("relay starts"));
            addSpace(1);
            lcd.setCursor(0, 2);
            lcd.print (F("adjust"));
            addSpace(1);
            lcd.write(2);
            addSpace(1);
            lcd.print (humiditySwing);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 19:
          editMode = editMode - 2;
          break;
        case 21:

          while (escape == 0) {
            checkeditbutton();
            buttons(5);
            set_minute = constrain(set_minute, 1, 59);
            lcd.setCursor(0, 0);
            showclock();
            lcd.setCursor(0, 1);
            lcd.print(standardText[0]);
            lcd.setCursor(0, 2);
            lcd.print (F("enter minute:"));
            addSpace(1);
            if (set_minute < 10) {
              addSpace(1);
            }
            lcd.print(set_minute);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
          }
          editMode = 0;
          clock_update = 1;
          break;
        case 22:
          //set hour here

          while (escape == 0) {
            checkeditbutton();
            buttons(6);
            set_hour = constrain(set_hour, 1, 24);
            lcd.setCursor(0, 0);
            lcd.print (F("set hour"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[0]);
            lcd.setCursor(0, 2);
            lcd.print (F("enter hour:"));
            addSpace(1);
            if (set_hour < 10) {
              addSpace(1);
            }
            lcd.print(set_hour);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
          }
          editMode = 0;
          clock_update = 1;
          break;
        case 23:

          while (escape == 0) {
            checkeditbutton();
            buttons(7);
            set_day = constrain(set_day, 1, 31);
            lcd.setCursor(0, 0);
            lcd.print (F("set day"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[0]);
            lcd.setCursor(0, 2);
            lcd.print (F("enter day"));
            addSpace(1);
            if (set_day < 10) {
              addSpace(1);
            }
            lcd.print(set_day);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
          }
          editMode = 0;
          clock_update = 1;
          break;
        case 24:

          while (escape == 0) {
            checkeditbutton();
            buttons(8);
            set_month = constrain(set_month, 1, 12);
            lcd.setCursor(0, 0);
            lcd.print (F("set month"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[0]);
            lcd.setCursor(0, 2);
            lcd.print (F("enter month"));
            addSpace(1);
            lcd.print (set_month);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
          }
          editMode = 0;
          clock_update = 1;
          break;
        case 25:

          while (escape == 0) {
            checkeditbutton();
            buttons(9);
            set_year = constrain(set_year, 2018, 2030);
            //set year here
            lcd.setCursor(0, 0);
            lcd.print (F("set year"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[0]);
            lcd.setCursor(0, 2);
            lcd.print (F("enter year"));
            addSpace(1);
            lcd.print(set_year);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
          }
          editMode = 0;
          clock_update = 1;

          break;
        case 26:
          editMode = editMode - 2;
          break;
        case 31:
          tunerDisableScreen();
          break;
        case 32:
          editMode = editMode - 2;
          break;
        case 41:
          //set how many days to turn alarm
          while (escape == 0) {
            checkeditbutton();
            buttons(4);
            hatchDays = constrain(hatchDays, 1, 40);
            lcd.setCursor(3, 0);
            lcd.print (F("days before"));
            lcd.setCursor(2, 1);
            lcd.print (F("lockdown"));
            lcd.setCursor(0, 2);
            lcd.print (F("adjust "));
            addSpace(1);
            lcd.print (hatchDays);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 42:
          //add how many days to count and start
          lcd.clear();
          lcd.setCursor(1, 2);
          lcd.print (F("countdown started"));
          delay(1000);
          hatchCountUp = 0;
          memory_update = 1;
          editMode = editMode - 1;
          break;
        case 43:
          //disable count down alarm
          // hatchAlarm==1 on
          while (escape == 0) {
            checkeditbutton();
            buttons(16);

            lcd.setCursor(3, 0);
            lcd.print (F("hatch alarm"));
            lcd.setCursor(1, 1);
            lcd.print (F("on/off"));

            lcd.setCursor(12, 1);
            if (hatchAlarm == 0) {
              lcd.print (F("off"));
            }
            else {
              lcd.print (F("on"));
              addSpace(1);
            }
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;

          break;
        case 44:
          editMode = editMode - 2;
          break;
        case 51:

          while (escape == 0) {
            checkeditbutton();
            buttons(17);
            if (turnerType > 4) {
              turnerType = 1;
            }
            if (turnerType < 1) {
              turnerType = 4;
            }
            lcd.setCursor(3, 0);
            lcd.print (F("turner design"));
            lcd.setCursor(2, 1);
            switch (turnerType) {
              case 1:
                lcd.print (F("always running"));
                break;
              case 2:
                lcd.print (F("timed on/delay"));
                break;
              case 3:
                addSpace(3);
                lcd.print (F("cam tilt"));
                addSpace(3);
                break;
              case 4:
                lcd.print (F("reversing tilt"));
                break;
            }
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;

        case 52:
          //select f or c
          prevtempinF = tempinF;
          while (escape == 0) {
            checkeditbutton();
            buttons(31);
            lcd.setCursor(3, 0);
            lcd.print (F("temp F or C"));
            lcd.setCursor(8, 1);
            lcd.print (F("select"));

            lcd.setCursor(4, 2);
            if ( tempinF  == 0) {
              lcd.print (F("temp in F"));
            }
            else {
              lcd.print (F("temp in C"));
              addSpace(1);
            }
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
          }

          if (tempinF != prevtempinF) {
            if (tempinF == 0) {
              temp_setpoint = 99.5;
              highTempSetPoint = 102;
              lowTempAlarmPoint = 95;
              cal_temp = 0;
              tempSwing = 0.5;
            } else {
              temp_setpoint = 37.5;
              highTempSetPoint = 39;
              lowTempAlarmPoint = 33;
              cal_temp = 0;
              tempSwing = 0.5;
            }
            EEPROM.put(0, temp_setpoint);//float
            EEPROM.put(8, highTempSetPoint);//float
            EEPROM.put(12, lowTempAlarmPoint);//float
            EEPROM.write(40, tempinF);
            EEPROM.put(48, cal_temp);
            EEPROM.put(56, tempSwing);

            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print("change detected");
            lcd.setCursor(1, 1);
            lcd.print("check calibration");
            lcd.setCursor(5, 3);
            lcd.print("returning");
            delay(3000);
          }
          editMode = editMode - 1;
          break;

        case 53:
          //to next edit mode disable or enable audio alarm
          while (escape == 0) {
            checkeditbutton();
            buttons(23);
            lcd.setCursor(3, 0);
            lcd.print (F("audio alarm"));
            lcd.setCursor(1, 1);
            lcd.print (F("on/off"));

            lcd.setCursor(12, 1);
            if (audioalarm  == 0) {
              lcd.print (F("off"));
            }
            else {
              lcd.print (F("on"));
              addSpace(1);
            }
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 54:
          //to next edit mode add hotspot to home display
          lcd.clear();
          lcd.setCursor(8, 0);
          lcd.print (F("sry"));
          lcd.setCursor(2, 1);
          lcd.print (F("not available"));
          lcd.setCursor(2, 2);
          lcd.print (F("on this model"));
          lcd.setCursor(4, 3);
          lcd.print (F("returning"));
          delay(3000);
          editMode = editMode - 1;
          break;

        case 55:
          editMode = editMode - 2;
          break;
        case 61:
          //write to memory setup numbers
          while (escape == 0) {
            checkeditbutton();
            buttons(30);
            lcd.setCursor(3, 0);
            lcd.print (F("reset"));
            lcd.setCursor(1, 1);
            lcd.print (F("yes/no"));

            lcd.setCursor(12, 1);
            if (facReset == 0) {
              lcd.print (F("no"));
              addSpace(1);
            }
            else {
              lcd.print (F("yes"));
            }
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
          }
          if (facReset == 0) {
            editMode = editMode - 1;
          } else {
            lcd.clear();
            lcd.setCursor(5, 1);
            lcd.print (F("saving"));
            facReset = 0;
            if (tempinF == 0) {// is F
              temp_setpoint = 99.5;
              highTempSetPoint = 102;
              lowTempAlarmPoint = 95;
            }
            else {//is C
              temp_setpoint = 37.5;
              highTempSetPoint = 39;
              lowTempAlarmPoint = 33;
            }
            humidity_setpoint = 35;
            turnerAlarm = 1;
            highHumidAlarmPoint = 85;
            lowHumidAlarmPoint = 25;
            cal_temp = 0;
            cal_humidity = 0;
            tempSwing = 0.5;
            humiditySwing = 1;
            hatchDays = 19;
            hatchCountUp = 0;
            turnerType = 2;
            tunerDisable = 0;
            TimeBetweenTurns = 2;
            TimeTurnerTurns = 15;
            memory_update = 1;
            noSave = 0;
            audioalarm = 1;
            tempalarm = 1;
            humalarm = 1;
            hatchAlarm = 1;
            memoryUpdater();
            delay(2000);
            editMode = 0;
          }
          break;
        case 62:
          editMode = editMode - 2;
          break;
        case 81:
          //temp alarm on/off  all linked back to alarmcheck()

          while (escape == 0) {
            checkeditbutton();
            buttons(24);
            lcd.setCursor(3, 0);
            lcd.print (F("temp alarm"));
            lcd.setCursor(1, 1);
            lcd.print (F("on/off"));

            lcd.setCursor(12, 1);
            if (tempalarm  == 0) {
              lcd.print (F("off"));
            }
            else {
              lcd.print (F("on"));
              addSpace(1);
            }
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;


          break;
        case 82:
          //humidity alarm on/off linked back to alarmcheck()

          while (escape == 0) {
            checkeditbutton();
            buttons(25);
            lcd.setCursor(2, 0);
            lcd.print (F("humidity alarm"));
            lcd.setCursor(1, 1);
            lcd.print (F("on/off"));

            lcd.setCursor(12, 1);
            if (humalarm   == 0) {
              lcd.print (F("off"));
            }
            else {
              lcd.print (F("on"));
              addSpace(1);
            }
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 83:
          //display set points
          while (escape == 0) {
            checkeditbutton();
            lcd.setCursor(0, 0);
            lcd.print(F("high low set points"));
            lcd.setCursor(0, 1);
            lcd.print(F("L"));
            lcd.write(1);
            addSpace(1);
            lcd.print (lowTempAlarmPoint, 1);
            addSign();
            lcd.setCursor(10, 1);
            lcd.print(F("H"));
            lcd.write(1);
            addSpace(1);
            lcd.print (highTempSetPoint, 1);
            addSign();
            lcd.setCursor(0, 2);
            lcd.print("L");
            lcd.write(2);
            addSpace(1);
            lcd.print (lowHumidAlarmPoint, 0);
            lcd.setCursor(10, 2);
            lcd.print(F("H"));
            lcd.write(2);
            addSpace(1);
            lcd.print (highHumidAlarmPoint, 0);
            lcd.setCursor(0, 3);
            lcd.print(standardText[3]);
          }
          editMode = editMode - 1;
          break;
        case 84:
          //over temp setpoint linked back to alarmcheck()
          while (escape == 0) {
            checkeditbutton();
            buttons(26);
            lcd.setCursor(2, 0);
            lcd.print (F("high temp alarm"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[2]);
            lcd.print (highTempSetPoint, 1);
            addSign();
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;

          break;
        case 85:
          //under temp setpoint linked back to alarmcheck()
          while (escape == 0) {
            checkeditbutton();
            buttons(27);
            lcd.setCursor(2, 0);
            lcd.print (F("low temp alarm"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[2]);
            lcd.print (lowTempAlarmPoint, 1);
            addSign();
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 86:
          //high hum setpoint linked back to alarmcheck()
          while (escape == 0) {
            checkeditbutton();
            buttons(28);
            lcd.setCursor(2, 0);
            lcd.print (F("high humid alarm"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[2]);
            lcd.print (highHumidAlarmPoint, 0);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 87://low hum setpoint
          while (escape == 0) {
            checkeditbutton();
            buttons(29);
            lcd.setCursor(2, 0);
            lcd.print (F("low humid alarm"));
            lcd.setCursor(0, 1);
            lcd.print(standardText[2]);
            lcd.print (lowHumidAlarmPoint, 0);
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 88://return
          editMode = editMode - 2;
          break;
        case 101:
          //disable
          tunerDisableScreen();
          break;
        case 102:
          // setTimeTurnerTurns
          while (escape == 0) {
            checkeditbutton();
            buttons(19);
            if (TimeTurnerTurns < 1) {
              TimeTurnerTurns = 1;
            }
            lcd.setCursor(3, 0);
            lcd.print (F("turner runs for"));
            lcd.setCursor(2, 1);
            lcd.print (TimeTurnerTurns);
            addSpace(1);
            lcd.print (F("seconds"));
            lcd.setCursor(0, 2);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 103:
          //time between turns
          setTimeBetweenTurns();
          break;
        case 104:
          //test turner
          turntester();
          break;
        case 105:
          //return
          editMode = editMode - 2;
          break;
        case 111:
          //disable
          tunerDisableScreen();
          break;
        case 112:
          //time between turns
          setTimeBetweenTurns();
          break;
        case 113:
          //cam time
          while (escape == 0) {
            checkeditbutton();
            buttons(19);
            if (TimeTurnerTurns < 1) {
              TimeTurnerTurns = 1;
            }
            lcd.setCursor(3, 0);
            lcd.print (F("time to clear"));
            lcd.setCursor(5, 1);
            lcd.print (F("cam switch"));
            lcd.setCursor(2, 2);
            lcd.print (TimeTurnerTurns);
            addSpace(1);
            lcd.print (F("seconds"));
            lcd.setCursor(0, 3);
            lcd.print(standardText[1]);
            memory_update = 1;
          }
          editMode = editMode - 1;
          break;
        case 114:
          //turn test
          turntester();
          break;
        case 115:
          editMode = editMode - 2;
          break;
        case 121:
          //disable
          tunerDisableScreen();
          break;
        case 122:
          //time between turns
          setTimeBetweenTurns();
          break;
        case 123:
          //turner manual
          while (escape == 0) {
            checkeditbutton();
            buttons(22);
            if (minusButton == 0) {
              digitalWrite (turnerrelay1, ON);
            } else {
              digitalWrite (turnerrelay1, OFF);
            }

            if (plusButton == 0) {
              digitalWrite (turnerrelay2, ON);
            } else {
              digitalWrite (turnerrelay2, OFF);
            }

            lcd.setCursor(4, 0);
            lcd.print (F("tilt manual"));
            lcd.setCursor(2, 1);
            lcd.print (F("use up and down"));
            lcd.setCursor(2, 2);
            lcd.print (F("to move table"));
            lcd.setCursor(0, 3);
            lcd.print(standardText[3]);
            tunerDisable = 0;
          }
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print (F("remember to"));
          lcd.setCursor(2, 1);
          lcd.print (F("enable tilt"));
          lcd.setCursor(2, 2);
          lcd.print (F("when finnished"));
          lcd.setCursor(4, 3);
          lcd.print (F("returning"));
          delay(3000);
          editMode = editMode - 1;

          break;
        case 124:
          //turner alarm
          setturnerAlarm();
          break;
        case 125:
          //test turner
          turntester();
          break;
        case 126:
          editMode = editMode - 2;
          break;
      }
      lcd.clear();
      break;
  }
}

void buttons(byte type) {

  plusButton = digitalRead(buttonPlus);
  minusButton = digitalRead(buttonMinus);

  if ((plusButton == 0) && (prevplusButton == 1)) {
    lcd.clear();
    switch (type) {
      case 1:
        menuNumber++;
        break;
      case 2:
        menuNumber2++;
        break;
      case 3:
        menuNumber3++;
        break;
      case 4:
        hatchDays++;
        break;
      case 5:
        set_minute++;
        break;
      case 6:
        set_hour++;
        break;
      case 7:
        set_day++;
        break;
      case 8:
        set_month++;
        break;
      case 9:
        set_year++;
        break;
      case 10:
        temp_setpoint = temp_setpoint + 0.1;
        break;
      case 11:
        humidity_setpoint++;
        break;
      case 12:
        cal_temp =  cal_temp + 0.1;
        break;
      case 13:
        cal_humidity = cal_humidity + 0.1;
        break;
      case 14:
        tempSwing = tempSwing + 0.1;
        break;
      case 15:
        humiditySwing = humiditySwing + 1;
        break;
      case 16:
        hatchAlarm = ! hatchAlarm;
        break;
      case 17:
        turnerType++;
        break;
      case 18:
        TimeBetweenTurns++;
        break;
      case 19:
        TimeTurnerTurns++;
        break;
      case 20:
        turnerAlarm = !turnerAlarm;
        break;
      case 21:
        tunerDisable = !tunerDisable;
        break;
      case 22:
        //  do not use this spot
        break;
      case 23:
        audioalarm = ! audioalarm;
        break;
      case 24:
        tempalarm = !tempalarm;
        break;
      case 25:
        humalarm = ! humalarm;
        break;
      case 26:
        highTempSetPoint = highTempSetPoint + 0.1;
        break;
      case 27:
        lowTempAlarmPoint = lowTempAlarmPoint + 0.1;
        break;
      case 28:
        highHumidAlarmPoint = highHumidAlarmPoint + 1;
        break;
      case 29:
        lowHumidAlarmPoint = lowHumidAlarmPoint + 1;
        break;
      case 30:
        facReset = !facReset;
        break;
      case 31:
        tempinF = !tempinF;
        break;
    }
    prevplusButton = 0;
  }
  if (plusButton == 1) {
    prevplusButton = 1; //debounce
  }

  if ((minusButton == 0) && (prevMinusButton == 1)) {
    lcd.clear();
    switch (type) {
      case 1:
        menuNumber--;
        break;
      case 2:
        menuNumber2--;
        break;
      case 3:
        menuNumber3--;
        break;
      case 4:
        hatchDays--;
        break;
      case 5:
        set_minute--;
        break;
      case 6:
        set_hour--;
        break;
      case 7:
        set_day--;
        break;
      case 8:
        set_month--;
        break;
      case 9:
        set_year--;
        break;
      case 10:
        temp_setpoint = temp_setpoint - 0.1;
        break;
      case 11:
        humidity_setpoint--;
        break;
      case 12:
        cal_temp =  cal_temp - 0.1;
        break;
      case 13:
        cal_humidity = cal_humidity - 0.1;
        break;
      case 14:
        tempSwing = tempSwing - 0.1;
        break;
      case 15:
        humiditySwing = humiditySwing - 1;
        break;
      case 16:
        hatchAlarm = !hatchAlarm;
        break;
      case 17:
        turnerType--;
        break;
      case 18:
        TimeBetweenTurns--;
        break;
      case 19:
        TimeTurnerTurns--;
        break;
      case 20:
        turnerAlarm = !turnerAlarm;
        break;
      case 21:
        tunerDisable = !tunerDisable;
        break;
      case 22:
        //do not use this spot
        break;
      case 23:
        audioalarm = ! audioalarm;
        break;
      case 24:
        tempalarm = !tempalarm;
        break;
      case 25:
        humalarm = ! humalarm;
        break;
      case 26:
        highTempSetPoint = highTempSetPoint - 0.1;
        break;
      case 27:
        lowTempAlarmPoint = lowTempAlarmPoint - 0.1;
        break;
      case 28:
        highHumidAlarmPoint = highHumidAlarmPoint - 1;
        break;
      case 29:
        lowHumidAlarmPoint = lowHumidAlarmPoint - 1;
        break;
      case 30:
        facReset = !facReset;
        break;
      case 31:
        tempinF = !tempinF;
        break;

    }
    prevMinusButton = 0;
  }
  if (minusButton == 1) {
    prevMinusButton = 1; //debounce
  }

}



void callHomeScreen() {

  lcd.setCursor(0, 0);
  showclock();
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.print(F(":"));
  addSpace(1);
  if (adj_temp, 1 < 100) {
    addSpace(1); //stop screen moving due to extra digit
  }
  lcd.print (adj_temp, 1);
  addSign();
  addSpace(1);
  lcd.write(2);
  lcd.print(F(":"));
  addSpace(1);
  lcd.print (adj_humidity, 1);

  lcd.setCursor(0, 2);
  lcd.write (4);
  switch (display_stat_turn) {
    case 1:
      addSpace(6);
      lcd.print (F("off"));
      addSpace(8);
      break;
    case 2:

      time_to_turn_hr = (turn_timer / 60);
      time_to_turn_mins = turn_timer - (time_to_turn_hr * 60);
      if ((time_to_turn_hr == 0) && (time_to_turn_mins == 0)) {
        lcd.print (F(" turn pending "));
        lcd.print (countdown);
      }
      else {
        //add stop screen moving due to extra digit
        addSpace(1);
        lcd.print (time_to_turn_hr);
        addSpace(1);
        lcd.print (F("hrs"));
        addSpace(1);

        if (time_to_turn_mins < 10) {
          addSpace(1);
        }
        lcd.print (time_to_turn_mins);
        addSpace(1);
        lcd.print (F("mins"));
      }

      break;
    case 3:
      if ((turnerType == 1) || (turnerType == 4)) {
        addSpace(5);
        lcd.print (F("running"));
        addSpace(6);
      } else {
        addSpace(2);
        lcd.print (F("running"));
        addSpace(1);
        lcd.print (turnerRunCounter);
      }
      break;
  }

  lcd.setCursor(0, 3);
  if (alarm_active == 0) {
    if (hatchAlarm == 1) {
      lcd.print (F("hatch in"));
      addSpace(1);
      lcd.print (hatchDays);
      addSpace(1);
      lcd.print (F("days"));
      addSpace(2);
    } else {
      lcd.write (3);
      addSpace(5);
      lcd.print  (F("enabled"));
      addSpace(5);
    }
  }


  if (alarm_active != 0) {
    lcd.write (3);
    switch (alarm_active) {
      case 1:
        addSpace(4);
        lcd.print (F("high temp"));
        addSpace(4);
        break;
      case 2:
        addSpace(4);
        lcd.print (F("low temp"));
        addSpace(5);
        break;
      case 3:
        addSpace(4);
        lcd.print (F("hi humidity"));
        addSpace(2);
        break;
      case 4:
        addSpace(3);
        lcd.print (F("low humidity"));
        addSpace(2);
        break;
      case 7:
        addSpace(4);
        lcd.print (F("rtc fail"));
        addSpace(4);
        break;
      case 8:
        addSpace(2);
        lcd.print (F("turner overrun"));
        addSpace(1);
        break;
      case 9:
        addSpace(1);
        lcd.print (F("lockdown now"));
        addSpace(1);
        break;

    }

  }

  lcd.setCursor(18, 3);
  if (heaterdisplay == 1) {
    lcd.write (1);//temp relay indicator
  } else {
    addSpace(1);
  }
  lcd.setCursor(19, 3);
  if (humidityfandisplay == 1) {
    lcd.write (2);//humidity relay indicator
  } else {
    addSpace(1);
  }
}

void getTemp() {
  if (temp_timer >= 1) {
    //sensor1.temperature().getSensor(&sensor);
    humidity = sensor1.readHumidity();
    // Read temperature as Celsius
    tempC = sensor1.readTemperature();
    // Read temperature as Fahrenheit
    tempF = sensor1.readTemperature(true);
    temp_timer = 0;
  }

  if (tempinF == 0) {
    temp = tempF;
  }
  else {
    temp = tempC;
  }
  // failed to read sensor alarm
  if (isnan(humidity) || isnan(temp)) {
    alarm_active = 5;
    badsensor = 1; //bad reading go safe
  } else {
    badsensor = 0;
  }



  adj_humidity = humidity - cal_humidity;
  adj_temp = temp - cal_temp;



}
void humidityFan() {
  //humidity fan code
  if (badsensor == 0) { //good reading
    if (adj_humidity >= humidity_setpoint) {
      digitalWrite (humidityRelay, OFF);//off
      humidityfandisplay = 0;
    }
    else if ( adj_humidity <= humidity_setpoint - humiditySwing ) {
      if (editMode != 0) {
        digitalWrite (humidityRelay, OFF);//in while loop disable
      } else {
        digitalWrite (humidityRelay, ON);//low is on
        humidityfandisplay = 1;
      }
    }

  } else { //bad reading go safe
    digitalWrite (humidityRelay, OFF);//off
    humidityfandisplay = 0;
  }
}

void heatercontrols() {
  if (badsensor == 0) { //good reading
    if (adj_temp >= temp_setpoint) {
      digitalWrite (heaterRelay, OFF);//off
      heaterdisplay = 0;
    }
    else if ( adj_temp <= temp_setpoint - tempSwing ) {
      if (editMode != 0) {
        digitalWrite (heaterRelay, OFF);//in while loop disable
      } else {
        digitalWrite (heaterRelay, ON);//low is on
        heaterdisplay = 1;
      }
    }

  } else { //bad reading go safe
    digitalWrite (heaterRelay, OFF);//off
    heaterdisplay = 0;
  }
}

void  checkeditbutton() {

  lightButton = digitalRead(buttonEnter);
  editButton = digitalRead(buttonEdit);

  //after 2 mins kick out of menu and do not save changes
  unsigned long currentMillis5 = millis();
  if (currentMillis5 - previousMillis5 > 120000L) {
    escape = 1;
    noSave = 1;
    editMode = 0;
    previousMillis5 = currentMillis5;
  }

  if (editButton != preveditButton) {
    if (editButton == LOW) {
      editMode++;
      escape = 1;
      lcd.clear();
      previousMillis5 = currentMillis5;
    }
    preveditButton = editButton;
  }
  editMode = constrain(editMode, 0, 3);

  if (lightButton != prevlightButton) {
    if (lightButton == LOW ) {
      editMode = 0;
      lcd.clear();
      escape = 1;
      noSave = 1;
      menuNumber = 0;
      subMenu = 0;
      setlightrelay = !setlightrelay;
    }
    prevlightButton = lightButton;
  }

  if (setlightrelay == 0) {
    digitalWrite(lightrelay, HIGH);
  } else {
    digitalWrite(lightrelay, LOW);
  }
}


void showclock() {
  dt = clock.getDateTime();
  //clock.getTime();
  if (dt.hour < 10) {
    lcd.print("0");
  }
  lcd.print(dt.hour, DEC);
  lcd.print(":");
  if (dt.minute < 10) {
    lcd.print("0");
  }
  lcd.print(dt.minute, DEC);
  lcd.print(":");
  if (dt.second < 10) {
    lcd.print("0");
  }
  lcd.print(dt.second, DEC);
  lcd.print(" ");
  if (dt.day < 10) {
    lcd.print("0");
  }
  lcd.print(dt.day, DEC);
  lcd.print("/");
  if (dt.month < 10) {
    lcd.print("0");
  }
  lcd.print(dt.month, DEC);
  lcd.print("/");
  lcd.print(dt.year, DEC);
}
void memoryUpdater() {
  if (noSave == 1) {
    memory_update = 0;
  }
  noSave = 0;
  if (memory_update == 1) {
    EEPROM.put(0, temp_setpoint);//float
    EEPROM.write (4, turnerAlarm );// int
    EEPROM.put(8, highTempSetPoint);//float
    EEPROM.put(12, lowTempAlarmPoint);//float
    EEPROM.put(16, highHumidAlarmPoint);//float
    EEPROM.put(20, lowHumidAlarmPoint);//float
    EEPROM.write(24, audioalarm );
    EEPROM.write(28, tempalarm );
    EEPROM.write(32, humalarm );
    EEPROM.write(36, hatchAlarm );
    EEPROM.write(40, tempinF);
    EEPROM.put(48, cal_temp);
    EEPROM.put(52, cal_humidity);
    EEPROM.put(56, tempSwing);
    EEPROM.put(60, humiditySwing);
    EEPROM.write(64, hatchDays );
    EEPROM.write(68, hatchCountUp );
    EEPROM.write(72, turnerType );
    EEPROM.write(76, tunerDisable );
    EEPROM.write(80, TimeBetweenTurns );
    EEPROM.write(84, TimeTurnerTurns);
    EEPROM.put(88, humidity_setpoint);
  }
  memory_update = 0;
}
void turnerCountForDisplay() {
  dt = clock.getDateTime();
  if (reboot == 1) {
    prev_clock_day = dt.day;
    reboot = 0;
  }
  if (prev_clock_day != dt.day) {
    hatchCountUp++;
    hatchDays = hatchDays - hatchCountUp;
  }

  prev_clock_day = dt.day;
}
void alarmcheck () {

  alarm_active = 0;
  if (adj_temp >= highTempSetPoint ) {
    alarm_active = 1;
    if (tempalarm == 1) {
      soundalarm = 1;
    } else {
      soundalarm = 0;
    }
  }
  if (adj_temp <= lowTempAlarmPoint) {
    alarm_active = 2;
    if (tempalarm == 1) {
      soundalarm = 1;
    } else {
      soundalarm = 0;
    }
  }
  if (adj_humidity >= highHumidAlarmPoint) {
    alarm_active = 3;
    if (humalarm == 1) {
      soundalarm = 1;
    } else {
      soundalarm = 0;
    }
  }
  if (adj_humidity <= lowHumidAlarmPoint) {
    alarm_active = 4;
    if (humalarm == 1) {
      soundalarm = 1;
    } else {
      soundalarm = 0;
    }
  }
  if (hatchDays == 0) {
    alarm_active = 9;
    hatchAlarm;
    if (hatchAlarm == 1) {
      soundalarm = 1;
    } else {
      soundalarm = 0;
    }
  }
  if (audioalarm == 0) {
    soundalarm = 0;
  }

  if ((alarm_active != 0) && (soundalarm == 1)) {
    digitalWrite (alarmrelay, ON);
    tone(11, 1000);
  } else {
    digitalWrite (alarmrelay, OFF);
    soundalarm = 0;
    noTone(11);
  }
}

void clockset() {
  if (clock_update == 1) {
    //clock.fillByYMD(set_year, set_month, set_day); //Jan 19,2013
    //clock.fillByHMS(set_hour, set_minute, 30); //15:28 30"
    //clock.fillDayOfWeek(SAT);//Saturday
    clock.setDateTime(set_year, set_month, set_day, set_hour, set_minute, 30);
    //clock.setTime();//write time to the RTC chip
    clock_update = 0;
  }
}


void tunerDisableScreen() {
  while (escape == 0) {
    checkeditbutton();
    buttons(21);
    lcd.setCursor(3, 0);
    lcd.print (F("turner off/on"));
    lcd.setCursor(1, 1);
    lcd.print (F("on/off"));

    lcd.setCursor(12, 1);
    if (tunerDisable == 0) {
      lcd.print (F("off"));
    }
    else {
      lcd.print (F("on"));
      addSpace(1);
    }
    lcd.setCursor(0, 3);
    lcd.print(standardText[1]);
  }
  memory_update = 1;
  lcd.clear();
  editMode = editMode - 1;
}
void setTimeBetweenTurns() {
  while (escape == 0) {
    checkeditbutton();
    buttons(18);
    lcd.setCursor(1, 0);
    lcd.print (F("time between turns"));
    lcd.setCursor(0, 1);
    lcd.print (F("set in hours :"));
    addSpace(1);
    lcd.print (TimeBetweenTurns);
    lcd.setCursor(0, 3);
    lcd.print(standardText[1]);
    memory_update = 1;
  }
  editMode = editMode - 1;
}
void setturnerAlarm() {
  while (escape == 0) {
    if (tunerDisable == 0) {
      lcd.setCursor(3, 0);
      lcd.print (F("turner is"));
      lcd.setCursor(3, 1);
      lcd.print (F("disabled"));
      delay(3000);
      turnerAlarm = 0;
      escape = 1;
      editMode = editMode - 1;
    }



    checkeditbutton();
    buttons(20);
    lcd.setCursor(3, 0);
    lcd.print (F("turner off/on"));
    lcd.setCursor(1, 1);
    lcd.print (F("on/off"));

    lcd.setCursor(12, 1);
    if (turnerAlarm == 0) {
      lcd.print (F("off"));
    }
    else {
      lcd.print (F("on"));
      addSpace(1);
    }
    lcd.setCursor(0, 3);
    lcd.print(standardText[1]);
    memory_update = 1;
  }
  lcd.clear();
  editMode = editMode - 1;
}
void turnercontrols() {
  turnerstop = digitalRead(turnerinput);
  turnerEstop = digitalRead(overrun);

  timeBetweenTurnsSeconds = TimeBetweenTurns * 3600L;
  turn_timer = timeBetweenTurnsSeconds - secondTurnercounter;
  countdown = turn_timer;
  turn_timer = turn_timer / 60;

  if ((tunerDisable == 1) && (editMode == 0)) {
    switch (turnerType) {
      case 1:
        //always running
        digitalWrite (turnerrelay1, ON);
        display_stat_turn = 3;
        break;
      case 2://same as case 3
      case 3://cam signal input used on case 3
        timeBetweenTurnsSeconds = TimeBetweenTurns * 3600L;
        turn_timer = timeBetweenTurnsSeconds - secondTurnercounter;
        countdown = turn_timer;
        turn_timer = turn_timer / 60;
        if ((secondTurnercounter >= timeBetweenTurnsSeconds) || (testTurner == 1)) { //time to turn
          if (turnerRunCounter <= TimeTurnerTurns) {//turn for this time
            digitalWrite (turnerrelay1, ON);
            display_stat_turn = 3;
          } else {//done turning
            if ((turnerstop == 0) || (turnerType == 2)) {
              digitalWrite (turnerrelay1, OFF);//high is off
              secondTurnercounter = 0;//reset time to turn
              testTurner = 0;
            }
          }
        } else {//not time to turn prime turn for this time
          turnerRunCounter = 0;
          display_stat_turn = 2;
        }
        break;
      case 4:
        //reversing tilt
        if (turnerEstop == 1) {
          timeBetweenTurnsSeconds = TimeBetweenTurns * 3600L;
          turn_timer = timeBetweenTurnsSeconds - secondTurnercounter;
          countdown = turn_timer;
          turn_timer = turn_timer / 60;
          if ((secondTurnercounter >= timeBetweenTurnsSeconds) || (testTurner == 1)) { //time to turn
            tilt = !tilt;
            onetime = 1;
            display_stat_turn = 3;
            secondTurnercounter = 0;//reset time to turn
            testTurner = 0;
          } else {
            turnerRunCounter = 0;
            display_stat_turn = 2;
          }
          if ((tilt == 1) && ((turnerstop == 1) || (onetime == 1))) {
            digitalWrite (turnerrelay1, ON);
            digitalWrite (turnerrelay2, OFF);
            display_stat_turn = 3;
          } if ((tilt == 0) && ((turnerstop == 1) || (onetime == 1))) {
            digitalWrite (turnerrelay1, OFF);
            digitalWrite (turnerrelay2, ON);
            display_stat_turn = 3;
          }
          if (prevturnerstop != turnerstop) {
            onetime = 0;
            prevturnerstop = turnerstop;
          }
          if ((turnerstop == 0) && (onetime == 0)) {
            digitalWrite (turnerrelay1, OFF);
            digitalWrite (turnerrelay2, OFF);
            display_stat_turn = 2;

          }

        } else {     //if (turnerEstop == 1) {
          digitalWrite (turnerrelay1, OFF);
          digitalWrite (turnerrelay2, OFF);
          alarm_active = 8;
        }
        break;

      default :
        display_stat_turn = 1;
        break;
    }
  } else {
    display_stat_turn = 1;
    digitalWrite (turnerrelay1, OFF);
    digitalWrite (turnerrelay2, OFF);
  }
}


void turntester() {
  while (escape == 0) {
    checkeditbutton();
    lcd.setCursor(5, 1);
    lcd.print (F("press enter"));
    lcd.setCursor(5, 2);
    lcd.print (F("to test"));
  }
  testTurner = 1;
  editMode = 0;
}
void timers() {
  unsigned long currentMillis = millis();//one second timers
  if (currentMillis - previousMillis > interval) {
    temp_timer++;//slow call to sensors
    secondTurnercounter++;//delay between turns
    turnerRunCounter++;
    previousMillis = currentMillis;
  }
}


void addSign() {
  if (tempinF == 0) {
    lcd.write(6);//6 is F sign
  }
  else {
    lcd.write(7);//7 is c sign
  }
}

void addSpace(int number) {
  for (int i = 1; i <= number; i++) {
    lcd.print(" ");
  }
}


//void showprint() {
// static unsigned long prevMillis1;
// unsigned long currentMillis1 = millis();
// if (currentMillis1 - prevMillis1 >= 500) {
//   Serial.print("tilt ");
//  Serial.println(tilt);
//  Serial.print("onetime ");
//  Serial.println(onetime);
//  Serial.print("testTurner ");
//  Serial.println(testTurner);
//  Serial.print("turnerstop ");
//  Serial.println(turnerstop);

//  prevMillis1 = currentMillis1;
// }
//}
