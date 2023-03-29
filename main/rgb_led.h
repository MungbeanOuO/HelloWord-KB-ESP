#ifndef _RGB_LED_H_
#define _RGB_LED_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

// Define the number of RGB LEDs
#define RGB_LED_NUM 75

// Define the RGB LED effect types
#define RGB_LED_EFFECT_NONE 0
#define RGB_LED_EFFECT_BREATH 1
#define RGB_LED_EFFECT_RAINBOW 2

// Define the RGB LED effect parameters
#define RGB_LED_EFFECT_BREATH_PERIOD 2000 // ms
#define RGB_LED_EFFECT_RAINBOW_PERIOD 1000 // ms

// Define the SPI device handle for RGB LED
extern spi_device_handle_t rgb_led_spi;

// Initialize RGB LED driver
void rgb_led_init(void);

// Update RGB LED effects based on key states
void rgb_led_update(uint8_t *key_states);

// Change RGB LED effect type based on HID command
void rgb_led_change_effect(uint8_t *hid_report);

#endif