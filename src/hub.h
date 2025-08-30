/**
 * @file hub.h
 * @brief Main hub interface for the E-Paper Portrait project.
 *
 * This header defines the core data structures and functions for managing
 * the e-paper display, battery sensor, SD card, and image list.
 *
 * Structures:
 * - ImageNode: Linked list node for storing image file paths.
 * - e_paper_hub_peripherals_config_t: Configuration for screen, battery, and SD card peripherals.
 * - e_paper_hub_hardware_status_t: Status flags for hardware components.
 * - e_paper_hub_system_stats_t: Aggregated system state, including configs, status, image list, frame buffer, and battery level.
 *
 * Globals:
 * - e_paper_hub_dev: Global instance holding the current system state.
 *
 * Functions:
 * - hub_initialize(): Initializes the hub with the given peripheral settings.
 * - hub_render_next_image(): Renders the next image from the image list to the display.
 */
#pragma once

#include "drivers/battery/max17048_i2c_driver.h"
#include "drivers/display/waveshare_42in_spi_driver.h"
#include "drivers/sdcard/sd_spi_driver.h"
#include "screen/frame.h"

/**
 * @brief Represents a node in a singly linked list of image file paths.
 *
 * This structure is used to store the path to an image file and a pointer to the next node in the list.
 */
struct ImageNode {
  /**
   * @brief A pointer to a character array (string) containing the file path of the image.
   */
  char* path;

  /**
   * @brief A pointer to the next ImageNode in the linked list, or NULL if this is the last node.
   */
  struct ImageNode* next;
};
typedef struct ImageNode ImageNode;

/**
 * @brief Configuration structure for e-paper hub peripherals.
 *
 * This structure aggregates the configuration settings for the main peripherals
 * used in the e-paper hub, including the screen, battery reader, and SD card.
 */
typedef struct {
  /**
   * @brief Configuration for the WS42 e-paper screen driver.
   */
  ws42_driver_config_t screen_config;

  /**
   * @brief Configuration for the MAX17048 battery reader I2C driver.
   */
  max17048_i2c_driver_config_t battery_reader_config;

  /**
   * @brief Configuration for the SD card driver.
   */
  sdcard_driver_config_t sdcard_config;
} e_paper_hub_peripherals_config_t;

/**
 * @brief Structure representing the hardware status of the e-paper hub.
 *
 * This structure holds the status information for various hardware components
 * of the e-paper hub, including the screen, SD card, and battery sensor.
 */
typedef struct {
  /**
   * @brief Status of the e-paper screen (uint8_t).
   */
  uint8_t screen_status;

  /**
   * @brief Status of the SD card (uint8_t).
   */
  uint8_t sdcard_status;

  /**
   * @brief Status of the battery sensor (uint8_t).
   */
  uint8_t batt_sensor_status;
} e_paper_hub_hardware_status_t;

/**
 * @brief Structure holding system statistics and state for the e-paper hub.
 *
 * This structure encapsulates the configuration, hardware status, image list,
 * frame buffer, and battery level for the e-paper hub system.
 */
typedef struct {

  /**
   * @brief Configuration settings for the e-paper hub peripherals.
   */
  e_paper_hub_peripherals_config_t _settings;

  /**
   * @brief Current hardware status of the e-paper hub.
   */
  e_paper_hub_hardware_status_t status;

  /**
   * @brief Pointer to the linked list of images managed by the hub.
   */
  ImageNode* images;

  /**
   * @brief Frame buffer used for graphics rendering.
   */
  graphics_frame_buffer_t frame_buffer;

  /**
   * @brief Current battery level as a percentage (0-100).
   */
  uint8_t battery_level;
} e_paper_hub_system_stats_t;

/**
 * @brief External declaration of the e-paper hub system statistics structure.
 *
 * This variable provides access to the system statistics for the e-paper hub device.
 * It is defined elsewhere and should be used to monitor or manipulate the hub's
 * system state and metrics.
 *
 * @note Ensure that the definition of e_paper_hub_system_stats_t is included before using this variable.
 */
extern e_paper_hub_system_stats_t e_paper_hub_dev;

/**
 * @brief Initializes the e-paper hub with the specified peripheral configuration.
 *
 * This function sets up the e-paper hub using the provided settings structure,
 * configuring all necessary peripherals for operation.
 *
 * @param settings The configuration structure containing peripheral settings.
 */
void hub_initialize(const e_paper_hub_peripherals_config_t settings);

/**
 * @brief Renders the next image on the e-paper display.
 *
 * This function advances to and displays the next image in the sequence
 * managed by the e-paper hub.
 */
void hub_render_next_image(void);
