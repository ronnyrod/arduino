

#include <dht.h> //Available at https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib Thanks to https://github.com/RobTillaart

#define FIRMWARE_VERSION 1 //Firmaware version
#define DHT11_PIN 5 //DTH11 data pin
#define LED_PIN 13 //Indicator led
#define H_MAX_PIN 6 //HIGH: Humidity over maximum offset | LOW: Humidity under maximum offset
#define H_MIN_PIN 7 //HIGH: Humidity under minimun offset | LOW: Humidity over minimum offset
#define T_MAX_PIN 8 //HIGH: Temperature over maximum offset | LOW: Temperature under maximum offset
#define T_MIN_PIN 9 //HIGH: Temperature under minimun offset | LOW: Temperature over minimum offset

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
//Events
String evtHumOutRange = "HUMIDITY"; //Indicates humidity event
String msgHumOutRange = "Humidity out of range"; //Message event
String evtTempOutRange = "TEMPERATURE"; //Indicates temperature event
String msgTempOutRange = "Temperature out of range"; //Message
//Parameters
long lastReadTime = 0; //Last time from sensor was asked (ms)
long interval = 2000; //Reading periodicity (ms)
boolean controlHumidity = true; //Flag to control humidity
boolean controlTemperature = true; //Flag to control temperature
int maxHumidity = 60; //Maximum humidity offset
int minHumidity = 34; //Minimum humidity offset
int maxTemperature = 37; //Maximum temperature offset
int minTemperature = 26; //Minimum temperature offset

void setup()
{
  Serial.begin(9600); //Configures serial port
  pinMode(LED_PIN, OUTPUT);  
  pinMode(H_MAX_PIN, OUTPUT);
  pinMode(H_MIN_PIN, OUTPUT);
  pinMode(T_MAX_PIN, OUTPUT);
  pinMode(T_MIN_PIN, OUTPUT);
  inputString.reserve(256);
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
      printMSG(okHeader,cmdEnableHumCtrl,1,noMessage);      
    }else if (cmdDisableHumCtrl.equalsIgnoreCase(inputString)) {
      controlHumidity = false;
      printMSG(okHeader,cmdDisableHumCtrl,1,noMessage);      
    }else if (cmdEnableTemCtrl.equalsIgnoreCase(inputString)) {
      controlTemperature = true;
      printMSG(okHeader,cmdEnableTemCtrl,1,noMessage);      
    }else if (cmdDisableTemCtrl.equalsIgnoreCase(inputString)) {
      controlTemperature = false;
      printMSG(okHeader,cmdDisableTemCtrl,1,noMessage);      
    } else if (inputString.startsWith(cmdSetMaxHumidity) && (inputString.length() >= cmdSetMaxHumidity.length() + 2)) {
      inputString.replace(cmdSetMaxHumidity,"");
      maxHumidity = inputString.toInt();
      printMSG(okHeader,cmdSetMaxHumidity,maxHumidity,noMessage);
    } else if (inputString.startsWith(cmdSetMinHumidity) && (inputString.length() >= cmdSetMinHumidity.length() + 2)) {
      inputString.replace(cmdSetMinHumidity,"");
      minHumidity = inputString.toInt();
      printMSG(okHeader,cmdSetMinHumidity,minHumidity,noMessage);
    } else if (inputString.startsWith(cmdSetMaxTemperature) && (inputString.length() >= cmdSetMaxTemperature.length() + 2)) {
      inputString.replace(cmdSetMaxTemperature,"");
      maxTemperature = inputString.toInt();
      printMSG(okHeader,cmdSetMaxTemperature,maxTemperature,noMessage);
    } else if (inputString.startsWith(cmdSetMinTemperature) && (inputString.length() >= cmdSetMinTemperature.length() + 2)) {
      inputString.replace(cmdSetMinTemperature,"");
      minTemperature = inputString.toInt();
      printMSG(okHeader,cmdSetMinTemperature,minTemperature,noMessage);
    } else {
      printMSG(errHeader,cmdError,errCodeUnknownCmd,msgUnknownCmd);    
    }
    inputString = "";
    cmdReceived = false; 
  }  
  unsigned long currentMillis = millis();
  if (currentMillis - lastReadTime > interval) {
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
  }

 
  
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

