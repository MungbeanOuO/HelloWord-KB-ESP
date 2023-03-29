#ifndef _CUSTOM_HID_H_
#define _CUSTOM_HID_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/usb/device.h"

// Define the USB device handle for Custom HID
extern usb_device_handle_t custom_hid_dev;

// Initialize Custom HID driver
void custom_hid_init(void);

// Generate HID report data based on key states and HID protocol
void custom_hid_generate_report(uint8_t *key_states, uint8_t *hid_report);

// Send HID report data to host via USB interface
void custom_hid_send_report(uint8_t *hid_report);

// Receive HID report data from host via USB interface if any
esp_err_t custom_hid_receive_report(uint8_t *hid_report);

// Process HID report data based on HID protocol and send commands to other tasks via queues if needed
void custom_hid_process_report(uint8_t *hid_report);

#endif