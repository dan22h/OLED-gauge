#define diffPin 2
#define gearPin 0
#define NUM_READS 20
#include <Goldelox_Const4D.h>
#include <Goldelox_Const4DSerial.h>
#include <Goldelox_Serial_4DLib.h>
#include <SoftwareSerial.h>
#define DisplayTemp mySerial

SoftwareSerial mySerial(10, 9); // RX, TX
Goldelox_Serial_4DLib DisplayT(&DisplayTemp);

unsigned long time;
long previousMillis = 0;
int diffTemp;
int gearTemp;
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
int relayCont = 8;
int ignition = 7;
int lights = 6;
boolean displayOn = false;
const int dimmerLow = 4; //Change brightness here
const int dimmerHigh = 15; //Change brightness here
int dimmer = 15;
int returnvaldiff = 0;
int returnval = 0;
float voltage;


void setup() {
  //delay (1000); 
  while(digitalRead(ignition) == HIGH) {
    digitalWrite(relayCont, LOW);     
              }
 //Serial.begin(9600); 
  pinMode(relayCont, OUTPUT);
  pinMode(ignition, INPUT);
  pinMode(lights, INPUT);
  //delay (1000);
  digitalWrite(relayCont, HIGH); 
  delay (4000);
  DisplayTemp.begin(38400) ;
  DisplayT.TimeLimit4D = 2000;  // 2 second timeout on all commands
  DisplayT.media_Init();

  for (int i = dimmer;dimmer>0;dimmer--){
       DisplayT.gfx_Contrast(dimmer);
       delay(200);
  }
  
  ShutdownCheck(dimmer);  //Shutdown sequence for ignition off, before initialising main code// 
  

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
  readings[thisReading] = 0;
  }  
  
  for (int thisReading2 = 0; thisReading2 < numReadings; thisReading2++) {
  readings2[thisReading2] = 0;
  }  
  
  
  
  
  
}


void loop() {

   if (displayOn == false) {
      returnvaldiff=modeAveragediff();
      diffTemp = runningaveragediff(returnvaldiff);
      
      returnval=modeAveragegear();
      gearTemp = runningaverage(returnval);
      
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
        DisplayT.gfx_Contrast(dimmer);
        //delay (10);
        }    
        }
     if (digitalRead(lights) == LOW){
        if (dimmer > dimmerLow){
        dimmer = dimmer - 1;
        //delay (10);
        DisplayT.gfx_Contrast(dimmer);
        //delay (10);
        }
        else if (dimmer < dimmerLow){
        dimmer = dimmer + 1;
        //delay (10);
        DisplayT.gfx_Contrast(dimmer);
        //delay (10);
        }

        }
      returnvaldiff=modeAveragediff();
      diffTemp=runningaveragediff(returnvaldiff);
      OLEDsequencediff(diffTemp);
      
      returnval=modeAveragegear();
      gearTemp = runningaverage(returnval);
      OLEDsequencegear(gearTemp);
        if (digitalRead(ignition) == HIGH) {
          time = millis()+5000;
          while(millis() < time){
            returnvaldiff=modeAveragediff();
            diffTemp = runningaveragediff(returnvaldiff); 
            OLEDsequencediff(diffTemp); 
            
            returnval=modeAveragegear();
            gearTemp = runningaverage(returnval);
            OLEDsequencegear(gearTemp);
            if(digitalRead(ignition) == LOW) {
              break;   
              }
          }
          dimmer = ShutdownCheck(dimmer); 
              //Serial.print(dimmer);
              //Serial.print(" ");
        }
  }
  }






int ShutdownCheck(int dimmer1){
  dimmer = dimmer1;
 if (digitalRead(ignition) == HIGH) {
    //DisplayT.media_Init();
    //DisplayT.media_SetSector(0x0000, 0x0000);
    DisplayT.media_VideoFrame(0, 0, 1) ;
    DisplayT.media_VideoFrame(0, 64, 2) ;
    time = millis()+4000;
    while(millis() < time){
      if(digitalRead(ignition) == LOW) {
          return dimmer;
          //break; 
      }
    }
      for (int i = dimmer;dimmer>0;dimmer--){
        DisplayT.gfx_Contrast(dimmer);
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
        //digitalWrite(relayCont, HIGH);
    }
  }
  return dimmer;
}



void OLEDsequencediff(int Temp1){
     //dimmer = dimmer2;
     diffTemp = Temp1;

    if (diffTemp < -30) {
      DisplayT.media_VideoFrame(0, 64, 368) ;
    }
    else if (diffTemp > 150)  {
      DisplayT.media_VideoFrame(0, 64, 367) ;
    }
    else {
      diffValue = (diffTemp+216);
      //DisplayT.media_Init();
      //DisplayT.media_SetSector(0x0000, 0x0000) ;
      DisplayT.media_VideoFrame(0, 64, diffValue) ;
    }

      //Serial.print(Temp);
     //Serial.print("  ");
    
}



void OLEDsequencegear(int Temp2){

     gearTemp = Temp2;

    if (gearTemp < -30) {
      DisplayT.media_VideoFrame(0, 0, 185) ;
    }
    else if (gearTemp > 150)  {
      DisplayT.media_VideoFrame(0, 0, 184) ;
    }
    else {
      gearValue = (gearTemp+33);
      //DisplayT.media_Init();
      //DisplayT.media_SetSector(0x0000, 0x0000) ;
      DisplayT.media_VideoFrame(0, 0, gearValue) ;
    }
      //Serial.print(Temp);
     //Serial.print("  ");    
}



int modeAveragediff(){
  int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){     
     int value = analogRead(diffPin);
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
   //return scaled mode of 10 values
   float returnval = 0;
   for(int i=NUM_READS/2-5;i<(NUM_READS/2+5);i++){
     returnval +=sortedValues[i];
   }
   returnval = returnval/10;
   //voltage = (returnval * (5.00 / 1023.0))*1000;
   //diffTemp = (((10.888 - sqrt((sq(-10.888)) + (4 * 0.00347 * (1777.3 - voltage)))) / (2 * (-0.00347))) + 30);
   analogRead(gearPin);  // to pre select next sensor and allow the analogue readings to stabalise prior to using for next sensor
   return returnval;
}



int modeAveragegear(){
  int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){     
     int value = analogRead(gearPin);
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
   //return scaled mode of 10 values
   float returnval = 0;
   for(int i=NUM_READS/2-5;i<(NUM_READS/2+5);i++){
     returnval +=sortedValues[i];
   }
   returnval = returnval/10;
   //voltage = (returnval * (5.00 / 1023.0))*1000;
   //gearTemp = (((10.888 - sqrt((sq(-10.888)) + (4 * 0.00347 * (1777.3 - voltage)))) / (2 * (-0.00347))) + 30);
   analogRead(diffPin);  // to pre select next sensor and allow the analogue readings to stabalise prior to using for next sensor
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
   
   
  float voltagediff= (diffaverage * (5.00 / 1023.0))*1000;
  diffTemp = (((10.888 - sqrt((sq(-10.888)) + (4 * 0.00347 * (1777.3 - voltagediff)))) / (2 * (-0.00347))) + 30);
  return diffTemp;
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
   
   
   float voltagegear= (average * (5.00 / 1023.0))*1000;
  gearTemp = (((10.888 - sqrt((sq(-10.888)) + (4 * 0.00347 * (1777.3 - voltagegear)))) / (2 * (-0.00347))) + 30);
  return gearTemp;
}

