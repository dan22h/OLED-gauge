#define pressurePin 0
#define voltagePin 1
#define NUM_READS 20
#include <Goldelox_Const4D.h>
#include <Goldelox_Const4DSerial.h>
#include <Goldelox_Serial_4DLib.h>
#include <SoftwareSerial.h>
#define DisplayPres mySerial

SoftwareSerial mySerial(10, 9); // RX, TX
Goldelox_Serial_4DLib DisplayP(&DisplayPres);

unsigned long time;
long previousMillis = 0;
float pressure;
int volts;
//int voltageVariable;
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readings2[numReadings];      // the readings from the analog input
int readIndexdiff = 0;              // the index of the current reading
int totaldiff = 0;                  // the running total
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int diffaverage = 0; // the average
int average = 0;
int diffValue;
int gearValue;
int returnvaldiff = 0;
int returnval = 0;


boolean displayOn = false;
int relayCont = 8;
int ignition = 7;
int lights = 6;
int modeswitch = 2;
//int oilValue;
//float oilpressure;
//int offSwitch = 7;
const int dimmerLow = 4; //Change brightness here
const int dimmerHigh = 15; //Change brightness here
int dimmer = 15;



void setup() {
  while(digitalRead(ignition) == HIGH) {                
              }
  //Serial.begin(9600);
  pinMode(relayCont, OUTPUT);
  pinMode(ignition, INPUT);
  pinMode(lights, INPUT);
  pinMode(modeswitch, INPUT);
  digitalWrite(relayCont, HIGH); 
  delay (4000);
  DisplayPres.begin(38400) ;
  DisplayP.TimeLimit4D = 1000 ;  // 2 second timeout on all commands
  DisplayP.media_Init();

  for (int i = dimmer;dimmer>0;dimmer--){
       DisplayP.gfx_Contrast(dimmer);
       delay(200);
  }
  
  ShutdownCheck(dimmer);  //Shutdown sequence for ignition off, before initialising main code// 
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
  readings[thisReading] = 0;
  }
}

void loop()  {   
  
  if (displayOn == false) {
    returnval=modeAveragepressure();
    pressure= runningaverage(returnval);
    
    returnval=modeAveragevolts();
    volts = runningaveragediff(returnval);
    
    time = millis();
    if (time >= 8000){ //used to prevent initial 10 wrong readings 
      displayOn = true;
      }
      }
  if (displayOn == true) {
    if (digitalRead(lights) == HIGH) {
      if (dimmer < dimmerHigh){
        dimmer = dimmer + 1;
        //delay (10);
        DisplayP.gfx_Contrast(dimmer);
        //delay (10);
        }    
        }
     if (digitalRead(lights) == LOW){
        if (dimmer > dimmerLow){
        dimmer = dimmer - 1;
        //delay (10);
        DisplayP.gfx_Contrast(dimmer);
        //delay (10);
        }
        else if (dimmer < dimmerLow){
        dimmer = dimmer + 1;
        //delay (10);
        DisplayP.gfx_Contrast(dimmer);
        //delay (10);
        }
        }
        if (digitalRead (modeswitch) == HIGH){         
           returnval=modeAveragepressure();
           pressure= runningaverage(returnval); 
           OLEDpressure(pressure);
           
          if (digitalRead(ignition) == HIGH) {
            time = millis()+5000;
          }
          while(millis() < time){
           returnval=modeAveragepressure();
           pressure= runningaverage(returnval); 
           OLEDpressure(pressure);
            if(digitalRead(ignition) == LOW) {
              break;   
              }
            }
           dimmer = ShutdownCheck(dimmer); 
          } 
          else{
            returnval=modeAveragevolts();
            volts = runningaveragediff(returnval);           
            OLEDvoltage(volts);
            
            if (digitalRead(ignition) == HIGH) {
            time = millis()+5000;
          }
          while(millis() < time){
            returnval=modeAveragevolts();
            volts = runningaveragediff(returnval);           
            OLEDvoltage(volts); 
            if(digitalRead(ignition) == LOW) {
              break;   
              }
            }
          }
  }
}

  
  
  

 int ShutdownCheck(int dimmer1){
  dimmer = dimmer1;

 if (digitalRead(ignition) == HIGH) {
    //DisplayP.media_Init();
    //DisplayP.media_SetSector(0x0000, 0x0000);
    DisplayP.media_VideoFrame(0, 0, 1) ;
    time = millis()+4000;
    while(millis() < time){
      if(digitalRead(ignition) == LOW) {
          return dimmer;
          //break; 
      }
    }
      for (int i = dimmer;dimmer>0;dimmer--){
        DisplayP.gfx_Contrast(dimmer);
        time = millis()+200;
        while(millis() < time){
          if (digitalRead(ignition) == LOW) {
            return dimmer;
            //break; 
          }
        }   
      }
    time = millis()+2000;
    while(millis() < time){
      if (digitalRead(ignition) == LOW) {
        return dimmer;
        //break; 
      }
    }
      if (digitalRead(ignition) == HIGH) {
        digitalWrite(relayCont, LOW);
        delay(10000);
        digitalWrite(relayCont, HIGH);
    }
  }
  return dimmer;
}

void OLEDpressure(int pressure1){
   pressure = pressure1;
   //Serial.print(pressure);
   //Serial.print("  ");
  
   if (pressure <= 0) {
    DisplayP.media_VideoFrame(0, 0, 125) ;

  }
  else if (pressure > 60)  {
    DisplayP.media_VideoFrame(0, 0, 186) ;

  }
  else {
  //float oilValue = (pressure*10)+1;
  //DisplayP.media_Init();
  //DisplayP.media_SetSector(0x0000, 0x0000) ;
  DisplayP.media_VideoFrame(0, 0, (pressure+125)) ;

  }
}


void OLEDvoltage(int voltage1){
  volts = voltage1 - 57;
  if (voltage1 <60) {
    DisplayP.media_VideoFrame(0, 0, 2) ;
  }
  else if (voltage1 > 180)  {
    DisplayP.media_VideoFrame(0, 0, 124) ;

  }
   else {
  //DisplayP.media_Init();
  //DisplayP.media_SetSector(0x0000, 0x0000) ;
  DisplayP.media_VideoFrame(0, 0, volts) ;

  }
}


int modeAveragepressure(){
  int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){     
     int value = analogRead(pressurePin);
     int j;
     if(value<sortedValues[0] || i==0){
        j=0; //insert at first position
     }
     else{
       for(j=1;j<i;j++){
          if(sortedValues[j-1]<=value && sortedValues[j]>=value){
            // j is insert position
            break;
          }
       }
     }
     for(int k=i;k>j;k--){
       // move all values higher than current reading up one position
       sortedValues[k]=sortedValues[k-1];
     }
     sortedValues[j]=value; //insert current reading
   }
   //return scaled mode of 4 values
   float returnval = 0;
   for(int i=NUM_READS/2-5;i<(NUM_READS/2+5);i++){
     returnval +=sortedValues[i];
   }
   returnval = returnval/10;
   //return returnval;
   //float voltage= (returnval * (5.00 / 1023.0))*1000;  
   //pressure = ((1/40)*voltage)-(25/2);
   //float pressure = ((0.00173590130916415) * voltage) + (-0.916555891238671);
   //pressure = pressure*10; //corrects pressure to 1 decimal place
   //Serial.print (pressure);
   //Serial.print(" ");
   analogRead(voltagePin);
   return returnval;
}



int modeAveragevolts(){
  int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){     
     int value = analogRead(voltagePin);
     int j;
     if(value<sortedValues[0] || i==0){
        j=0; //insert at first position
     }
     else{
       for(j=1;j<i;j++){
          if(sortedValues[j-1]<=value && sortedValues[j]>=value){
            // j is insert position
            break;
          }
       }
     }
     for(int k=i;k>j;k--){
       // move all values higher than current reading up one position
       sortedValues[k]=sortedValues[k-1];
     }
     sortedValues[j]=value; //insert current reading
   }
   //return scaled mode of 4 values
   float returnval = 0;
   for(int i=NUM_READS/2-5;i<(NUM_READS/2+5);i++){
     returnval +=sortedValues[i];
   }
   returnval = returnval/10;
   //return returnval;
   //float voltage= (returnval * (5.00 / 1023.0));  
   //float voltageVariable = (4.0305*voltage);
   //voltageVariable = voltageVariable*10; //corrects pressure to 1 decimal place
   //Serial.print (pressure);
   //Serial.print(" ");
   analogRead(pressurePin);
   return returnval;
}








int runningaveragediff(int returnval1){
  returnvaldiff = returnval1;
  totaldiff = totaldiff - readings2[readIndexdiff];
  readings2[readIndexdiff] = returnvaldiff;// read from the sensor:
  totaldiff = totaldiff + readings2[readIndexdiff];// add the reading to the total:
  readIndexdiff = readIndexdiff + 1; // advance to the next position in the array:
  if (readIndexdiff >= numReadings) {// if we're at the end of the array...
    readIndexdiff = 0;// ...wrap around to the beginning:
  }
  diffaverage = totaldiff / numReadings;// calculate the average: 
   
   
  //float voltagediff= (diffaverage * (5.00 / 1023.0))*1000;
  //pressure = (((10.888 - sqrt((sq(-10.888)) + (4 * 0.00347 * (1777.3 - voltagediff)))) / (2 * (-0.00347))) + 30);
  
  float voltage= (returnval * (5.00 / 1023.0));  
  volts = (4.0305*voltage);
  volts = volts*10; //corrects pressure to 1 decimal place
  
  return pressure;
}



int runningaverage(int returnval1){
  returnval = returnval1;
  total = total - readings[readIndex];
  readings[readIndex] = returnval;// read from the sensor:
  total = total + readings[readIndex];// add the reading to the total:
  readIndex = readIndex + 1; // advance to the next position in the array:
  if (readIndex >= numReadings) {// if we're at the end of the array...
    readIndex = 0;// ...wrap around to the beginning:
  }
  average = total / numReadings;// calculate the average: 
  //gearTemp = (((10.888 - sqrt((sq(-10.888)) + (4 * 0.00347 * (1777.3 - voltagegear)))) / (2 * (-0.00347))) + 30);
  float voltage= (returnval * (5.00 / 1023.0))*1000;  
   pressure = ((1/40)*voltage)-(25/2);
   pressure = ((0.00173590130916415) * voltage) + (-0.916555891238671);
   pressure = pressure*10; //corrects pressure to 1 decimal place 
  return pressure;
}


