/* MPU_6050_Lib.h
* Version: 1.1
* Created: 2023-04-21
* Author: Hjalmar Stenberg & unknown
*
* Purpose: Library for interraction with MPU6050 
* System: ESP32 Feather V2
* Uses: Adafruit_MPU6050.h, Adafruit_Sensor.h, Wire.h
*******************************************************************************/

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

void initMPU6050();

void getReadingsMPU6050(sensors_event_t *a,sensors_event_t *g,sensors_event_t *temp);
