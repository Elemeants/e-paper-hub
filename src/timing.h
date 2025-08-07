#pragma once

#include <esp_timer.h>

#define WAIT_MS(__us__) (vTaskDelay(__us__ / portTICK_PERIOD_MS))

#define _KHZ(__khz__) (__khz__ * 1000)
#define _MHZ(__mhz__) (__mhz__ * _KHZ(1000))
