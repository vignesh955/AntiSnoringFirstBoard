#include <Arduino.h>
#include "watchDog.h"

/**
 * @brief Watchdog initialization
 * @param Timeout
 */

void initWatchdog(int timeout) {
    esp_task_wdt_init(timeout, true);  // Enable panic handler
    esp_task_wdt_add(NULL);  // Attach the main task to watchdog
}

/**
 * @brief Watchdog reset
 */

void feedWatchdog() {
    esp_task_wdt_reset();
}
