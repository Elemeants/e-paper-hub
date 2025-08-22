#include "hub.h"

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

#include "esp_log.h"
#include "hub.h"
#include "screen/renderer.h"
#include "test_image.h"
#include "utils/timing.h"

/** Private variables */

static const char* TAG = "hub";
static const char* LOGO =
    "  _____                      _    _       _     \n"
    "  |  __ \\                    | |  | |     | |    \n"
    "  | |__) |_ _ _ __   ___ _ __| |__| |_   _| |__  \n"
    "  |  ___/ _` | '_ \\ / _ \\ '__|  __  | | | | '_ \\ \n"
    "  | |  | (_| | |_) |  __/ |  | |  | | |_| | |_) |\n"
    "  |_|   \\__,_| .__/ \\___|_|  |_|  |_|\\__,_|_.__/ \n"
    "              | |                                \n"
    "              |_|                                \n";
static uint8_t __frame_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
static uint8_t __file_bitmap_buffer[CALC_INTERNAL_HEIGH * CALC_INTERNAL_WIDTH];
static graphics_frame_buffer_t frame_buffer = {
    .width = SCREEN_WIDTH,
    .height = SCREEN_HEIGHT,
    .buffer = __frame_buffer,
};

/** Public variables */

e_paper_hub_system_stats_t e_paper_hub_dev;

/** Private functions */

void _append_image_path(const char* path) {
  static ImageNode* last_node = NULL;

  const uint32_t path_length = strlen(path);
  ESP_LOGI(TAG, "   path_length: %d", path_length);

  ImageNode* new_node = (ImageNode*)malloc(sizeof(ImageNode));
  memset(new_node, 0x00, sizeof(ImageNode));
  char* new_path_buffer = (char*)malloc(path_length + 2U);
  memset(new_path_buffer, 0x00, path_length);

  strcpy(new_path_buffer, path);
  new_node->path = new_path_buffer;

  if (last_node) {
    last_node->next = new_node;
  }

  last_node = new_node;

  if (!e_paper_hub_dev.images) {
    e_paper_hub_dev.images = last_node;
  }
}

void _print_sys_info() {
  printf(LOGO);
  printf(" Firmware Version: %s\n", FIRMWARE_VERSION);
  printf(" Build time: %u\n", BUILD_TIME_UNIX);
}

void _driver_inits() {
  // Driver inits
  uint8_t err =
      max17048_i2c_driver_init(e_paper_hub_dev._settings.battery_reader_config);
  if (err) {
    e_paper_hub_dev.status.batt_sensor_status = err;
  }

  err = ws42_driver_init(e_paper_hub_dev._settings.screen_config);
  if (err) {
    ESP_LOGE(TAG, "Error initializing the screen driver, stopping!");
    e_paper_hub_dev.status.screen_status = err;
    return;
  }

  err = sdcard_driver_init(e_paper_hub_dev._settings.sdcard_config);
  if (err) {
    ESP_LOGE(TAG, "Error initializing the sdcard driver, stopping!");
    e_paper_hub_dev.status.sdcard_status = err;
  }
}

void _configure_frame_renderer() {
  e_paper_hub_dev.frame_buffer = frame_buffer;
  graphics_frame_buffer_clear(&frame_buffer, GRAPHICS_COLOR_WHITE);
  graphics_renderer_attach(&frame_buffer);
}

void _configure_image_loop() {
  DIR* d;
  struct dirent* dir;
  struct stat st;

  d = opendir(SD_VFS_MOUNTING_POINT);
  if (!d) {
    return;
  }

  while ((dir = readdir(d)) != NULL) {
    // Skip . and .. entries
    if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
      continue;
    }

    // Construct full path to check file type
    char full_path[1024];
    memset(full_path, 0x00, sizeof(full_path));
    snprintf(full_path, sizeof(full_path), "%s/%s", SD_VFS_MOUNTING_POINT,
             dir->d_name);

    if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
      ESP_LOGI(TAG, "File: %s", dir->d_name);
      const char* dot = strrchr(dir->d_name, '.');
      if (dot && (!strcmp(dot, ".BIN") || !strcmp(dot, ".bin"))) {
        ESP_LOGI(TAG, " _append_image_path(%s)", dir->d_name);
        _append_image_path(full_path);
      }
    }
  }
  closedir(d);

  ImageNode* node = e_paper_hub_dev.images;
  ESP_LOGI(TAG, "Images found");
  while (node) {
    ESP_LOGI(TAG, "- %s", node->path);
    node = node->next;
  }
}

/** Public functions */

void hub_initialize(const e_paper_hub_peripherals_config_t settings) {
  _print_sys_info();

  // Copy settings to the global struct
  memset(&e_paper_hub_dev, 0x00, sizeof(e_paper_hub_dev));
  e_paper_hub_dev._settings = settings;

  ESP_LOGI(TAG, "calling _driver_inits");
  _driver_inits();

  ESP_LOGI(TAG, "calling _configure_frame_renderer");
  _configure_frame_renderer();

  ESP_LOGI(TAG, "calling _configure_image_loop");
  _configure_image_loop();

  ESP_LOGI(TAG, "initialization finished!");
}

void hub_render_next_image() {
  static ImageNode* image;

  if (!image) {
    image = e_paper_hub_dev.images;
  }
  if (!image) {
    ESP_LOGE(TAG, "Critical error! no image found!");
    return;
  } else if (!image->path) {
    ESP_LOGE(TAG, "Critical error! no image path found!");
    return;
  }

  ESP_LOGI(TAG, "Reading file: %s", image->path);
  FILE* image_file = fopen(image->path, "r");
  if (!image_file) {
    ESP_LOGE(TAG, "Image in %s not found", image->path);
    return;
  }

  ESP_LOGI(TAG, "Reading image");
  memset(__file_bitmap_buffer, 0x00, sizeof(__file_bitmap_buffer));
  fread((char*)__file_bitmap_buffer, 1, sizeof(__file_bitmap_buffer), image_file);
  graphics_frame_buffer_clear(&e_paper_hub_dev.frame_buffer, GRAPHICS_COLOR_WHITE);
  graphics_frame_buffer_draw_bitmap(&e_paper_hub_dev.frame_buffer, 0, 21,
                                    (const uint8_t*)__file_bitmap_buffer,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    GRAPHICS_COLOR_BLACK);
  fclose(image_file);

  // Draw and render
  ESP_LOGI(TAG, "Drawing image");
  graphics_renderer_update();
  image = image->next;
}
