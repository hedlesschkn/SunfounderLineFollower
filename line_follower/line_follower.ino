//1-8 is 3.5" (88.9mm)
//1-2 is 0.5" (12.7mm)

/*
 * Sunfounder Line Follower Module to position
 * By: Tim Hebert & Daniel Lay
 * 6/27/2019
 * 
 * sunfounder official wiki:
 * http://wiki.sunfounder.cc/index.php?title=Line_Follower_Module
 * 
 * BUG:
 * thinner lines are non-linear
 * 
 */

#include <Wire.h>
#define uchar unsigned char
uchar t;
uchar data[16];
int idata[8];

//int minVal[8] = {35,58,56,58,61,68,66,34};
//int maxVal[8] = {110,155,159,134,161,162,153,102};

int minVal[8] = {99,99,99,99,99,99,99,99};
int maxVal[8] = {99,99,99,99,99,99,99,99};

int scaledData[8];
#define scaleMax 100
#define scaleMin 0

float distanceFromCenter;

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  t = 0;
  getWhiteBlack();
}
void loop()
{
readModule();
if(data[0] == 255){
  Serial.println("RESET!");
}

  scaleData();
  getPosition();

  //printRawTabbed();
  
  //printVisual();
  Serial.print(distanceFromCenter*1000);
  Serial.print("\t");
  Serial.print(3500);
  Serial.print("\t");
  Serial.println(0);
}
void readModule(){
    Wire.requestFrom(9, 16);    // request 16 bytes from slave device #9
  while (Wire.available())   // slave may send less than requested
  {
    data[t] = Wire.read(); // receive a byte as character
    if (t < 15)
      t++;
    else
      t = 0;
  }
  for(int i=0; i<8; i++){
    idata[i]=data[i*2];
  }
}
void printVisual(){
  
  for (int i=0; i<distanceFromCenter*20; i++){
    Serial.print(" ");
  }
  Serial.println("|");
}

void printTabbed(){
  for(int i=0; i<8; i++){
    Serial.print(scaledData[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void printRawTabbed(){
  for(int i=0; i<8; i++){
    Serial.print(idata[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void getWhiteBlack(){
  Serial.println("move the module across a black line");
  readModule();
  while( (idata[0] != 255 && idata[1] != 255) ){
        Serial.print(idata[0]);
        Serial.print("\t");
        Serial.print(idata[7]);
        Serial.print("\t");
        Serial.println(idata[7]-idata[0]);
        
        for(int j=0;j<8;j++){
        if(idata[j]<minVal[j] && idata[j]!=0){
          minVal[j] = idata[j];
        }
        if(idata[j]>maxVal[j] && idata[j]!=255){
          maxVal[j] = idata[j];
        }
      }
      readModule();
  }
  Serial.println("White and Black values obtained");
  Serial.print("Min: ");
  for(int i=0; i<8; i++){
    Serial.print(minVal[i]);
    Serial.print("\t");
  }
  Serial.print("Max: ");
  for(int i=0; i<8; i++){
    Serial.print(maxVal[i]);
    Serial.print("\t");
  }
  delay(5000);
  
}

void scaleData(){
  //scaledData[] = data[] as a percentage of the range
  for(int i=0;i<8;i++){
    //scaledData[i] = (scaleMax-scaleMin)*(data[i]-minVal[i])/(maxVal[i]-minVal[i])+scaleMin;
    //scaledData[i] = (scaleMin-scaleMax)*(data[i]-maxVal[i])/(minVal[i]-maxVal[i])+scaleMax;
  scaledData[i] = map(idata[i], minVal[i], maxVal[i], scaleMax, scaleMin);
  }
}

int getPosition(){
  //get max number
  int currMax = -99;
  int currMaxIndex = 9;
  for(int i=0;i<8;i++){
    if (scaledData[i]>currMax){
      currMax = scaledData[i];
      currMaxIndex = i;
    }
  }
//  Serial.println();
//  Serial.print(currMax);
//  Serial.print(",");
//  Serial.println(currMaxIndex);

  if (currMaxIndex >= 1 and currMaxIndex <= 6){ //calculate parabola if 3 points
    //quadradic formula: y=ax^2+bx+c
    //c = leftmost magnitude
    //x@left = 0, x@center=1, x@right=2
    
    float c = scaledData[currMaxIndex-1]; //magnitude of left neighbor
    float b = (4*scaledData[currMaxIndex]-scaledData[currMaxIndex+1]-3*c)/2;
    float a = scaledData[currMaxIndex]-c-b;
    float vertex = (-1*b)/(2*a); //vertex of x: -b/2a
    //distance from 0 to 2 is 1 inch
    distanceFromCenter = ((currMaxIndex-1)*0.5)+(vertex/2); //indexofleft*0.5"+distancefromleft
  }
  else{//calculate linearly if on the ends
    if(currMaxIndex = 0){ //far left
      if(scaledData[currMaxIndex+1] < 50){ //is left of far left
        //calculate slope, change in x over change in y
        float slope = scaledData[currMaxIndex]-scaledData[currMaxIndex+1]/.5;
        float posfromleft = (100/slope) + scaledData[currMaxIndex];
        //Serial.print("from left: ");
        //Serial.println(posfromleft);
      }
    }
      else{ //is left of far left
      }
  }
  //Serial.print("position in inches: ");
  //Serial.println(distanceFromCenter);
  
}
