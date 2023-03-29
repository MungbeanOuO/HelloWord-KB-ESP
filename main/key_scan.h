#ifndef _KEY_SCAN_H_
#define _KEY_SCAN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

// Define the number of keys
#define KEY_NUM 75

// Define the SPI device handle for key scan
extern spi_device_handle_t key_scan_spi;

// Initialize key scan driver
void key_scan_init(void);

// Scan keys and update key states
void key_scan_update(uint8_t *key_states);

#endif