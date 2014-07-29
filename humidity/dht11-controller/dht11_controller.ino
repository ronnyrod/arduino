#include <LiquidCrystal.h> //Arduino library
#include <EEPROM.h> //Arduino library
#include <dht.h> //Available at https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib Thanks to https://github.com/RobTillaart

#define FIRMWARE_VERSION 2 //Firmaware version
#define DHT11_PIN 10 //DTH11 data pin
#define LED_PIN 13 //Indicator led
#define H_MAX_PIN 6 //HIGH: Humidity over maximum offset | LOW: Humidity under maximum offset
#define H_MIN_PIN 7 //HIGH: Humidity under minimun offset | LOW: Humidity over minimum offset
#define T_MAX_PIN 8 //HIGH: Temperature over maximum offset | LOW: Temperature under maximum offset
#define T_MIN_PIN 9 //HIGH: Temperature under minimun offset | LOW: Temperature over minimum offset

//EEPROMM addresses
#define  MEM_ADDR_DT_VER  0
#define  MEM_ADDR_H_MAX  1
#define  MEM_ADDR_H_MIN  2
#define  MEM_ADDR_T_MAX  3
#define  MEM_ADDR_T_MIN  4
#define  MEM_ADDR_INTERVAL_L  5
#define  MEM_ADDR_INTERVAL_H  6
#define  MEM_ADDR_CRTL_H  7
#define  MEM_ADDR_CRTL_T  8

dht DHT; //DHT11 driver

int lastSensorState; //Last sensor state read ( 0: OK | <0: ERROR | check DTHLib Headers)
String inputString = ""; //String received from serial line
boolean cmdReceived = false; //Indicates command available to parse and process

String noMessage = ""; //Constant.  Empty message trick
//Message headers and separator
String okHeader = "OK"; //Positive response
String errHeader = "ERROR"; //Error response
String evtHeader = "EVENT"; //Controller event -
String separator = ":"; //Message separator 
//Error message, codes and bodies
String msgUnknownCmd = "unknown command";
String cmdError = "ERR";
int errCodeUnknownCmd = 0;
//Reset to default values
String cmdResetDefault = "RESET";//Reset data version to default values 
//Reading commands
String cmdReadHumidity = "READH";  //Requests for humidity last reading
String cmdReadTemperature = "READT"; //Requests for temperature last reading
String cmdReadSensorState = "READS"; //requests for sensor state last reading
String cmdVersion = "VERSION"; //Requests for firmware version
String cmdInfo = "INFO"; //Requests for controller information 
//Writting commands - 
String cmdForceReadSensor = "FORCE";
//Writting commands - control management 
String cmdEnableHumCtrl = "ONH"; //Enables humidity control
String cmdDisableHumCtrl = "OFFH"; //Disables humidity control
String cmdEnableTemCtrl = "ONT"; //Enables temperature control
String cmdDisableTemCtrl = "OFFT"; //Disables temperature control
//Writting commands - parameter modification commmands
String cmdSetMaxHumidity = "SETMAXH"; //Sets maximum humidity offset
String cmdSetMinHumidity = "SETMINH"; //Sets minimum humidity offset
String cmdSetMaxTemperature = "SETMAXT"; //Sets maximum temperature offset
String cmdSetMinTemperature = "SETMINT"; //Sets minimum temperature offset
//Writting commands - interval modification
String cmdSetInterval = "SETINT"; //Sets interval between sensor reading
//Events
String evtHumOutRange = "HUMIDITY"; //Indicates humidity event
String msgHumOutRange = "HR out of range"; //Message event
String evtTempOutRange = "TEMPERATURE"; //Indicates temperature event
String msgTempOutRange = "T out of range"; //Message
//Parameters
long lastReadTime = 0; //Last time from sensor was asked (ms)
int interval = 2000; //Reading periodicity (ms)
boolean controlHumidity = true; //Flag to control humidity
boolean controlTemperature = true; //Flag to control temperature
int maxHumidity = 60; //Maximum humidity offset
int minHumidity = 34; //Minimum humidity offset
int maxTemperature = 37; //Maximum temperature offset
int minTemperature = 26; //Minimum temperature offset
int dataVersion = 0; //Data version - EEPROM management

//LCD 
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
   //Set up the LCD: 
  lcd.begin(16, 2);
  
  //Show initial version
  lcd.setCursor(0, 0);
  lcd.print("Starting ...");
  lcd.setCursor(0, 1);
  lcd.print("Firmware v");
  lcd.print(FIRMWARE_VERSION);
  delay(1000);

  Serial.begin(9600); //Configures serial port
  pinMode(LED_PIN, OUTPUT);  
  pinMode(H_MAX_PIN, OUTPUT);
  pinMode(H_MIN_PIN, OUTPUT);
  pinMode(T_MAX_PIN, OUTPUT);
  pinMode(T_MIN_PIN, OUTPUT);
  inputString.reserve(256);
  
  //Recover data version
  dataVersion = EEPROM.read(MEM_ADDR_DT_VER);
  //dataVersion equals 0 means data was recorded from user
  if (dataVersion == 0) {
    maxHumidity = EEPROM.read(MEM_ADDR_H_MAX);
    minHumidity = EEPROM.read(MEM_ADDR_H_MIN);
    maxTemperature = EEPROM.read(MEM_ADDR_T_MAX);
    minTemperature = EEPROM.read(MEM_ADDR_T_MIN);
    interval = (long)(EEPROM.read(MEM_ADDR_INTERVAL_H)<<8)+EEPROM.read(MEM_ADDR_INTERVAL_L);
    controlTemperature = EEPROM.read(MEM_ADDR_CRTL_T);
    controlHumidity = EEPROM.read(MEM_ADDR_CRTL_H); 
  } else {
    //Default values
    interval = 2000; 
    controlHumidity = true; 
    controlTemperature = true; 
    maxHumidity = 60;
    minHumidity = 34;
    maxTemperature = 37;
    minTemperature = 26;
  }
}

void loop()
{
  //Parses received command and process requested action
  if (cmdReceived) {
    if (cmdReadHumidity.equalsIgnoreCase(inputString)) {
      printMSG(okHeader,cmdReadHumidity,DHT.humidity,noMessage);
    }else if (cmdReadTemperature.equalsIgnoreCase(inputString)) {
      printMSG(okHeader,cmdReadTemperature,DHT.temperature,noMessage);      
    }else if (cmdReadSensorState.equalsIgnoreCase(inputString)) { 
      printMSG(okHeader,cmdReadSensorState,lastSensorState,noMessage);     
    }else if (cmdForceReadSensor.equalsIgnoreCase(inputString)) {
      readSensor();
      printMSG(okHeader,cmdForceReadSensor,lastSensorState,noMessage);      
    }else if (cmdVersion.equalsIgnoreCase(inputString)) {
      printMSG(okHeader,cmdVersion,FIRMWARE_VERSION,noMessage);      
    }else if (cmdInfo.equalsIgnoreCase(inputString)) {
      printMSG(okHeader,cmdInfo,0,getInfo());      
    }else if (cmdEnableHumCtrl.equalsIgnoreCase(inputString)) {
      controlHumidity = true;
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdEnableHumCtrl,1,noMessage);      
    }else if (cmdDisableHumCtrl.equalsIgnoreCase(inputString)) {
      controlHumidity = false;
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdDisableHumCtrl,1,noMessage);      
    }else if (cmdEnableTemCtrl.equalsIgnoreCase(inputString)) {
      controlTemperature = true;
      write_range_eeprom(); //Saves values      
      printMSG(okHeader,cmdEnableTemCtrl,1,noMessage);      
    }else if (cmdDisableTemCtrl.equalsIgnoreCase(inputString)) {
      controlTemperature = false;
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdDisableTemCtrl,1,noMessage);      
    } else if (inputString.startsWith(cmdSetMaxHumidity) && (inputString.length() >= cmdSetMaxHumidity.length() + 1)) {
      inputString.replace(cmdSetMaxHumidity,"");
      maxHumidity = inputString.toInt();
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdSetMaxHumidity,maxHumidity,noMessage);
    } else if (inputString.startsWith(cmdSetMinHumidity) && (inputString.length() >= cmdSetMinHumidity.length() + 1)) {
      inputString.replace(cmdSetMinHumidity,"");
      minHumidity = inputString.toInt();
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdSetMinHumidity,minHumidity,noMessage);
    } else if (inputString.startsWith(cmdSetMaxTemperature) && (inputString.length() >= cmdSetMaxTemperature.length() + 1)) {
      inputString.replace(cmdSetMaxTemperature,"");
      maxTemperature = inputString.toInt();
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdSetMaxTemperature,maxTemperature,noMessage);
    } else if (inputString.startsWith(cmdSetMinTemperature) && (inputString.length() >= cmdSetMinTemperature.length() + 1)) {
      inputString.replace(cmdSetMinTemperature,"");
      minTemperature = inputString.toInt();
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdSetMinTemperature,minTemperature,noMessage);
    } else if (inputString.startsWith(cmdSetInterval) && (inputString.length() >= cmdSetInterval.length() + 1)) {
      inputString.replace(cmdSetInterval,"");
      interval = inputString.toInt();
      write_range_eeprom(); //Saves values
      printMSG(okHeader,cmdSetInterval,interval,noMessage);
    }  else if (cmdResetDefault.equalsIgnoreCase(inputString)) {
      EEPROM.write(MEM_ADDR_DT_VER,0xff);
      //Call setup again
      setup();
    } else {
      printMSG(errHeader,cmdError,errCodeUnknownCmd,msgUnknownCmd);    
    }
    inputString = "";
    cmdReceived = false; 
  }  
  unsigned long currentMillis = millis();
  if ((int)(currentMillis - lastReadTime) > interval) {
    readSensor();
    if(lastSensorState == 0) {
      if (controlHumidity) {
        if(DHT.humidity > maxHumidity) {
          digitalWrite(H_MAX_PIN, HIGH);
          digitalWrite(H_MIN_PIN, LOW);        
          printMSG(evtHeader,evtHumOutRange,DHT.humidity,msgHumOutRange);
        } else if(DHT.humidity < minHumidity) {
          digitalWrite(H_MAX_PIN, LOW);
          digitalWrite(H_MIN_PIN, HIGH);
          printMSG(evtHeader,evtHumOutRange,DHT.humidity,msgHumOutRange);
        } else {
          turnOffHumCtrl();
        }        
      } else {
        turnOffHumCtrl();        
      }
      if (controlTemperature) {
        if(DHT.temperature > maxTemperature) {
          digitalWrite(T_MAX_PIN, HIGH);
          digitalWrite(T_MIN_PIN, LOW);        
          printMSG(evtHeader,evtTempOutRange,DHT.temperature,msgTempOutRange);
        } else if (DHT.temperature < minTemperature) {
          digitalWrite(T_MAX_PIN, HIGH);
          digitalWrite(T_MIN_PIN, LOW);        
          printMSG(evtHeader,evtTempOutRange,DHT.temperature,msgTempOutRange);
        } else {
          turnOffTemCtrl();      
        }        
      } else {
        turnOffTemCtrl();        
      }   
    }
    //Print T and HR in LCD
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(DHT.temperature,1);
    lcd.print("C ");
    lcd.print("HR:");
    lcd.print(DHT.humidity,1);
    lcd.print("%");
    //Print T,H status
    lcd.setCursor(0, 1);
    if(digitalRead(T_MAX_PIN)==HIGH) {
      lcd.print("T>TMAX  ");
    } else if(digitalRead(T_MIN_PIN)==HIGH) {
      lcd.print("T<TMIN  ");
    } else {
      if (controlTemperature) {
        lcd.print("T = OK  ");
      } else {
        lcd.print("T  OFF  ");
      } 
    }
    if(digitalRead(H_MAX_PIN)==HIGH) {
      lcd.print("HR>HMAX");
    } else if(digitalRead(H_MIN_PIN)==HIGH) {
      lcd.print("HR<HMIN");
    } else {
      if (controlHumidity) {
        lcd.print("HR = OK");
      } else {
        lcd.print("HR  OFF");
      }
    }
  }

 
  
}
/*
Record data offsets into EEPROM
*/
void write_range_eeprom() {
  //Data version indicates user recorded data
  dataVersion = 0;
  //Write all ranges to eeprom
  EEPROM.write(MEM_ADDR_DT_VER,dataVersion);
  EEPROM.write(MEM_ADDR_H_MAX,maxHumidity);
  EEPROM.write(MEM_ADDR_H_MIN,minHumidity);
  EEPROM.write(MEM_ADDR_T_MAX,maxTemperature);
  EEPROM.write(MEM_ADDR_T_MIN,minTemperature);
  EEPROM.write(MEM_ADDR_INTERVAL_H,(interval>>8) & 0xff);
  EEPROM.write(MEM_ADDR_INTERVAL_L,(interval) & 0xff);
  EEPROM.write(MEM_ADDR_CRTL_T,controlTemperature);
  EEPROM.write(MEM_ADDR_CRTL_H,controlHumidity); 
}
/*
Turn off Humidity control management
*/
void turnOffHumCtrl() {
  digitalWrite(H_MAX_PIN, LOW);
  digitalWrite(H_MIN_PIN, LOW);  
}
/*
Turn off Temperature control management
*/
void turnOffTemCtrl() {
  digitalWrite(T_MAX_PIN, LOW);
  digitalWrite(T_MIN_PIN, LOW);  
}
/*
Send formatted response to serial port
*/
void printMSG(String header,String command,int result,String message) {
  Serial.print(header);
  Serial.print(separator);
  Serial.print(command);
  Serial.print(separator);
  
  if(message.length() == 0) {
    Serial.println(result, 1);
  } else {
    Serial.print(result, 1);
    Serial.print(separator);
    Serial.println(message);
  }  
  
}
/*
Build info message to answers INFO command
*/
String getInfo() {
  String output = String("interval:");
  output = output + interval;
  output = output + String(":data version:");
  output = output + dataVersion;
  if (controlHumidity) {
      output = output + String(":ctrl hum:ON [");
      output += minHumidity;
      output = output + String(",");
      output += maxHumidity;
      output = output + String("]");
  }else{
      output = output + String(":ctrl hum:OFF");    
  }
  if (controlTemperature) {
      output = output + String(":ctrl tem:ON [");
      output += minTemperature;
      output = output + String(",");
      output += maxTemperature;
      output = output + String("]");
  }else{
      output = output + String(":ctrl tem:OFF");    
  }
  return output;
}
/*
Call DTHLib to read humidity and temperature from sensor DHT11
*/
void readSensor() {
  digitalWrite(LED_PIN, HIGH);
  lastSensorState = DHT.read11(DHT11_PIN);
  lastReadTime = millis();
  digitalWrite(LED_PIN, LOW);
}
/*
Process incomming serial data
*/
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') {
      cmdReceived = true;
    }else{
      inputString += inChar;
      inputString.toUpperCase();
    }
  }
}

