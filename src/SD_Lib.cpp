/* SD_Lib.cpp
* Version: 1.3
* Created: 2023-04-21
* Author: Hjalmar Stenberg & unknown
*
* Purpose: Library for SD card interactions
* System: ESP32 Feather V2
* Uses: -
*******************************************************************************/

#include "SD_Lib.h"

SPIClass sdSPI(VSPI);

//Creating file arne
File arne;

void initSD(fs::FS &fs, bool WIPE_ON_RESET){
  sdSPI.begin();
  if(!SD.begin()){
    Serial.println("Card mount failed");
    return;
  }   
  Serial.println("1");
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  if(WIPE_ON_RESET){
    arne = fs.open("/arne.txt", FILE_WRITE);
    if(!arne){
        Serial.println("Failed to open Arne for writing");
        return;
    }
    if(arne.print("Wiping \n")){
        Serial.println("Arne is wiped");
    } else {
        Serial.println("Wipe failed");
    }
    arne.close();
  }
  return;
}

void openArne(fs::FS &fs){
  arne = fs.open("/arne.txt", FILE_APPEND);
  if(!arne){
    Serial.println("Failed to open arne for appending");
    return;
  }
}

void closeArne(){
  arne.close();
}

void printArne(const char * message){
  arne.print(message);
}
