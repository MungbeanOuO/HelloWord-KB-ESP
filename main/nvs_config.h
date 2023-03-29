#ifndef _NVS_CONFIG_H_
#define _NVS_CONFIG_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Define the NVS config size
#define NVS_CONFIG_SIZE 16

// Define the NVS config keys
#define NVS_CONFIG_KEY_LAYER 0
#define NVS_CONFIG_KEY_LED_EFFECT 1
#define NVS_CONFIG_KEY_LED_BRIGHTNESS 2
#define NVS_CONFIG_KEY_LED_SPEED 3

// Initialize NVS config driver
void nvs_config_init(void);

// Write NVS config data to NVS partition
void nvs_config_write(uint8_t *nvs_config);

// Read NVS config data from NVS partition
void nvs_config_read(uint8_t *nvs_config);

#endif