 /* Arne.ino
* Version: 1.4
* Created: 2023-04-21
* Author: Hjalmar Stenberg
*
* Purpose: Reads sensor data and writes it to SD card
* System: ESP32 Feather V2
* Uses: Mapf.h, ADC_Cal.h, SD_Lib.h, MPU6050_Lib.h
*******************************************************************************/

#include <Mapf.h>
#include "ADC_Cal.h"
#include "SD_Lib.h"
#include "MPU6050_Lib.h"

#define ACC_SCALE 200
#define MAX_VALUE_ADC 4095
#define PRINT_SERIAL 0
#define WIPE_ON_RESET 1
#define IS_TESTBENCH 0

//Arrays for ADC Calibration look-up tables
float ADC1_LUT[4096];
float ADC2_LUT[4096];

//Defining variables for error correction
float Error_X;
float Error_Y;
float Error_Z;

float Error_X200g;
float Error_Y200g;
float Error_Z200g;

float Error_Gyro_X;
float Error_Gyro_Y;
float Error_Gyro_Z;

void setup() {
  //Testbench-board has different errors and look-up tables
  if(IS_TESTBENCH){
    mempcpy(ADC1_LUT, ADC1_TB, sizeof(ADC1_TB[0])*4096);
    Error_X = -0.6033;
    Error_Y = 0.7590;
    Error_Z = -0.9998;

    Error_X200g = -0.42;
    Error_Y200g = -1.2015;
    Error_Z200g = 1.9084;

    Error_Gyro_X = 0;
    Error_Gyro_Y = 0;
    Error_Gyro_Z = 0;
  }
  else {
    mempcpy(ADC1_LUT, ADC1_KiB, sizeof(ADC1_KiB[0])*4096);
    Error_X = 0.4429;
    Error_Y = 6.0238;
    Error_Z = 1.3996;

    Error_X200g = 3.7216;
    Error_Y200g = 2.6667;
    Error_Z200g = 1.1978;

    Error_Gyro_X = 0;
    Error_Gyro_Y = 0;
    Error_Gyro_Z = 0;
  }

  //Initialize I2C communication
  Wire.begin(SDA,SCL,400000);
  
  //Initialize ADC
  analogReadResolution(12);
  Serial.begin(115200);

  //Initialize SD card reading
  initSD(SD,WIPE_ON_RESET);

  //Initialize MPU6050
  initMPU6050();
}


//Opens data-file, writes 5000 readings, then closes
void loop() {
  openArne(SD);
  for(int i=0;i<=5000;i++){
    //Get accelerometer readings
    float rawX = ADC1_LUT[analogRead(A2)];
    float rawY = ADC1_LUT[analogRead(A3)];
    float rawZ = ADC1_LUT[analogRead(A4)];

    //Mapping raw data to g-forces
    float scaledX = mapf(rawX, 0, MAX_VALUE_ADC, -ACC_SCALE, ACC_SCALE);
    float scaledY = mapf(rawY, 0, MAX_VALUE_ADC, -ACC_SCALE, ACC_SCALE);
    float scaledZ = mapf(rawZ, 0, MAX_VALUE_ADC, -ACC_SCALE, ACC_SCALE);

    //Fixing errors
    scaledX -= Error_X200g;
    scaledY -= Error_Y200g;
    scaledZ -= Error_Z200g;

    //Calculating total acceleration
    double accTot = sqrt(pow(scaledX,2) + pow(scaledY,2) + pow(scaledZ,2));

    //Make 200g acc readings to string
    char scaledXStr[16];
    dtostrf(scaledX,8,4,scaledXStr);
    char scaledYStr[16];
    dtostrf(scaledY,8,4,scaledYStr);
    char scaledZStr[16];
    dtostrf(scaledZ,8,4,scaledZStr);
    char accTotStr[16];
    dtostrf(accTot,8,4,accTotStr);

    //Get good acc readings
    sensors_event_t a, g, temp;
    getReadingsMPU6050(&a,&g,&temp);

    //Make good acc readings to string
    char goodAccXStr[16];
    dtostrf(a.acceleration.x-Error_X,8,4,goodAccXStr);
    char goodAccYStr[16];
    dtostrf(a.acceleration.y-Error_Y,8,4,goodAccYStr);
    char goodAccZStr[16];
    dtostrf(a.acceleration.z-Error_Z,8,4,goodAccZStr);

    //Make gyroscope readings to string
    char gyroX[16];
    dtostrf(g.gyro.x-Error_Gyro_X,8,4,gyroX);
    char gyroY[16];
    dtostrf(g.gyro.y-Error_Gyro_Y,8,4,gyroY);
    char gyroZ[16];
    dtostrf(g.gyro.z-Error_Gyro_Z,8,4,gyroZ);    

    //Get timestamp
    int currentTime = micros();
    char currentTimeStr[16];
    itoa(currentTime,currentTimeStr,10);
  
    //Write to file
    printArne(scaledXStr);
    printArne(",");
    printArne(scaledYStr);
    printArne(",");
    printArne(scaledZStr);
    printArne(",");
    printArne(accTotStr);
    printArne(",");
    printArne(goodAccXStr);
    printArne(",");
    printArne(goodAccYStr);
    printArne(",");
    printArne(goodAccZStr);
    printArne(",");
    printArne(currentTimeStr);
    printArne("\n");

    //Printing to serial (mostly for testing purposes)
    if(PRINT_SERIAL){
//      Serial.print("Constant:");
//      Serial.print(1);
//      Serial.print(",");
      Serial.print("X:");
      Serial.print(goodAccXStr);
      Serial.print(",");
      Serial.print("Y:");
      Serial.print(goodAccYStr);
      Serial.print(",");
      Serial.print("Z:");
      Serial.println(goodAccZStr);
      
//      Serial.print(",");
//      Serial.print("GyroX:");
//      Serial.print(gyroX);
//      Serial.print(",");
//      Serial.print("GyroY:");
//      Serial.print(gyroY);
//      Serial.print(",");
//      Serial.print("GyroZ:");
//      Serial.println(gyroZ);
    }
  }
  closeArne();
}
