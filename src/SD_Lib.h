/* SD_Lib.h
* Version: 1.3
* Created: 2023-04-21
* Author: Hjalmar Stenberg & unknown
*
* Purpose: Library for SD card interactions
* System: ESP32 Feather V2
* Uses: FS.h, SD.h, SPI.h, sd_defines.h, sd_diskio.h
*******************************************************************************/

#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <sd_defines.h>
#include <sd_diskio.h>

void initSD(fs::FS &fs, bool WIPE_ON_RESET);

void openArne(fs::FS &fs);

void closeArne();

void printArne(const char * message);
