/**
 * @file timing.h
 * @author jdanypa@gmail.com (Elemeants)
 * @brief Timing utilities for FreeRTOS
 *
 * This header file provides macros and functions for timing operations in FreeRTOS,
 * including delays and frequency conversions.
 */
#pragma once

#include <esp_timer.h>

/**
 * @brief Wait for a specified number of milliseconds
 *
 * This macro uses FreeRTOS's vTaskDelay to pause the task for the specified number of milliseconds.
 *
 * @param __us__ Number of milliseconds to wait
 */
#define sleep_ms(__us__) (vTaskDelay(__us__ / portTICK_PERIOD_MS))

/**
 * @brief Convert kilohertz to hertz
 *
 * This macro converts a frequency in kilohertz to hertz.
 *
 * @param __khz__ Frequency in kilohertz
 * @return Frequency in hertz
 */
#define _KHZ(__khz__) (__khz__ * 1000)

/**
 * @brief Convert megahertz to hertz
 *
 * This macro converts a frequency in megahertz to hertz.
 *
 * @param __mhz__ Frequency in megahertz
 * @return Frequency in hertz
 */
#define _MHZ(__mhz__) (__mhz__ * _KHZ(1000))
