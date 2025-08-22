/**
 * @file sd_spi_driver.c
 * @author jdanypa@gmail.com (Elemeants)
 */
#include "sd_spi_driver.h"

#include <driver/sdmmc_host.h>
#include <driver/sdspi_host.h>
#include <esp_log.h>
#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>

#include "utils/timing.h"

/** Private variables */

static const char TAG[] = "sd_spi_driver";
static sdcard_driver_config_t sdcard_config;
static sdmmc_card_t *sdcard_card = NULL;

/** Public variables */

const char* SD_VFS_MOUNTING_POINT = "/sdcard";

/** Private functions */

static void _spi_bus_initialize() {
  spi_bus_config_t buscfg = {.miso_io_num = sdcard_config.miso_pin,
                             .mosi_io_num = sdcard_config.mosi_pin,
                             .sclk_io_num = sdcard_config.clk_pin,
                             .quadwp_io_num = -1,
                             .quadhd_io_num = -1,
                             .max_transfer_sz = 4096};
  esp_err_t ret;
  ret = spi_bus_initialize(sdcard_config.host, &buscfg, SPI_DMA_CH_AUTO);
  ESP_ERROR_CHECK(ret);
}

static void _spi_device_initialize() {
  sdmmc_host_t host_config = SDSPI_HOST_DEFAULT();
  host_config.slot = (int)sdcard_config.host;
  host_config.max_freq_khz = _MHZ(20) / _KHZ(1);

  sdspi_device_config_t sd_dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  sd_dev_config.host_id = host_config.slot;
  sd_dev_config.gpio_cs = sdcard_config.cs_pin;

  esp_vfs_fat_sdmmc_mount_config_t mount_config = VFS_FAT_MOUNT_DEFAULT_CONFIG();
  mount_config.format_if_mount_failed = false;
  mount_config.max_files = 5;
  mount_config.allocation_unit_size = 0;

  esp_err_t ret = esp_vfs_fat_sdspi_mount(SD_VFS_MOUNTING_POINT, &host_config, &sd_dev_config, &mount_config, &sdcard_card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG,
               "Failed to mount filesystem. "
               "If you want the card to be formatted, set the "
               "CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(TAG,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
  }
}

/** Public functions */

uint8_t sdcard_driver_init(const sdcard_driver_config_t config) {
  sdcard_config = config;

  _spi_bus_initialize();
  _spi_device_initialize();
  if (!sdcard_card) {
    return ESP_FAIL;
  }

  sdmmc_card_print_info(stdout, sdcard_card);
  return ESP_OK;
}
