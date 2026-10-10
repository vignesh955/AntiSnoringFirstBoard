
#ifndef WDT_MANAGER_H
#define WDT_MANAGER_H

#include <Arduino.h>
#include <esp_task_wdt.h>

void initWatchdog(int timeout);
void feedWatchdog();

#endif  // WDT_MANAGER_H
