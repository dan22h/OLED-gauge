#define tempPin 0
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
int Temp;
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0; // the average
int oilValue;
int relayCont = 8;
int ignition = 7;
int lights = 6;
boolean displayOn = false;
const int dimmerLow = 4; //Change brightness here
const int dimmerHigh = 15; //Change brightness here
int dimmer = 15;
int returnval = 0;


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
}


void loop() {
  returnval=modeAverage();
  Temp = runningaverage(returnval);
   if (displayOn == false) {
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
        OLEDsequence(Temp);
        if (digitalRead(ignition) == HIGH) {
          time = millis()+5000;
          while(millis() < time){
            returnval=modeAverage();
            Temp = runningaverage(returnval); 
            OLEDsequence(Temp); 
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

void OLEDsequence(int Temp1){
     //dimmer = dimmer2;
     Temp = Temp1;

    if (Temp < -30) {
      DisplayT.media_VideoFrame(0, 0, 184) ;
    }
    else if (Temp > 150)  {
      DisplayT.media_VideoFrame(0, 0, 183) ;
    }
    else {
      oilValue = (Temp+32);
      //DisplayT.media_Init();
      //DisplayT.media_SetSector(0x0000, 0x0000) ;
      DisplayT.media_VideoFrame(0, 0, oilValue) ;
    }

      //Serial.print(Temp);
     //Serial.print("  ");
    
}

int modeAverage(){
  int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){     
     int value = analogRead(tempPin);
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
   return returnval;
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
   
   
   float voltage= (average * (5.00 / 1023.0))*1000;
  Temp = (((10.888 - sqrt((sq(-10.888)) + (4 * 0.00347 * (1777.3 - voltage)))) / (2 * (-0.00347))) + 30);
  return Temp;
}


