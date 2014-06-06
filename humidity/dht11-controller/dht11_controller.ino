

#include <dht.h>

dht DHT;
int lastSensorState;
String inputString = "";
boolean cmdReceived = false;

#define FIRMWARE_VERSION 1

String noMessage = "";
String okHeader = "OK";
String errHeader = "ERROR";
String evtHeader = "EVENT";
String separator = ":";
//Errors
String msgUnknownCmd = "unknown command";
String cmdError = "ERR";
int errCodeUnknownCmd = 0;
//Commands
String cmdReadHumidity = "READH";
String cmdReadTemperature = "READT";
String cmdReadSensorState = "READS";
String cmdForceReadSensor = "FORCE";
String cmdVersion = "VERSION";
String cmdInfo = "INFO";
//Humidity and temperature control 
String cmdEnableHumCtrl = "ONH";
String cmdDisableHumCtrl = "OFFH";
String cmdEnableTemCtrl = "ONT";
String cmdDisableTemCtrl = "OFFT";
//parameter modification commmands
String cmdSetMaxHumidity = "SETMAXH";
String cmdSetMinHumidity = "SETMINH";
String cmdSetMaxTemperature = "SETMAXT";
String cmdSetMinTemperature = "SETMINT";
//Events
String evtHumOutRange = "HUMIDITY";
String msgHumOutRange = "Humidity out of range";
String evtTempOutRange = "TEMPERATURE";
String msgTempOutRange = "Temperature out of range";
//Parameters
long lastReadTime = 0;
long interval = 2000; //2 seg
boolean controlHumidity = true;
boolean controlTemperature = true;
int maxHumidity = 60;
int minHumidity = 34;
int maxTemperature = 37;
int minTemperature = 26;

#define DHT11_PIN 5
#define LED_PIN 13
#define H_MAX_PIN 6
#define H_MIN_PIN 7
#define T_MAX_PIN 8
#define T_MIN_PIN 9
void setup()
{
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(H_MAX_PIN, OUTPUT);
  pinMode(H_MIN_PIN, OUTPUT);
  pinMode(T_MAX_PIN, OUTPUT);
  pinMode(T_MIN_PIN, OUTPUT);
  inputString.reserve(256);
}

void loop()
{
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
void turnOffHumCtrl() {
  digitalWrite(H_MAX_PIN, LOW);
  digitalWrite(H_MIN_PIN, LOW);  
}
void turnOffTemCtrl() {
  digitalWrite(T_MAX_PIN, LOW);
  digitalWrite(T_MIN_PIN, LOW);  
}
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
void readSensor() {
  digitalWrite(LED_PIN, HIGH);
  lastSensorState = DHT.read11(DHT11_PIN);
  lastReadTime = millis();
  digitalWrite(LED_PIN, LOW);
}

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

