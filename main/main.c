#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "key_scan.h"
#include "rgb_led.h"
#include "custom_hid.h"
#include "nvs_config.h"

#define TAG "MAIN"

// SPI pins for key scan and RGB LED
#define SPI_MOSI 13
#define SPI_MISO 12
#define SPI_CLK 14
#define SPI_CS 15

// Key scan task handle and queue
static TaskHandle_t key_scan_task_handle = NULL;
static QueueHandle_t key_scan_queue = NULL;

// RGB LED task handle and queue
static TaskHandle_t rgb_led_task_handle = NULL;
static QueueHandle_t rgb_led_queue = NULL;

// Custom HID task handle and queue
static TaskHandle_t custom_hid_task_handle = NULL;
static QueueHandle_t custom_hid_queue = NULL;

// NVS config task handle and queue
static TaskHandle_t nvs_config_task_handle = NULL;
static QueueHandle_t nvs_config_queue = NULL;

// Key scan task function
void key_scan_task(void *pvParameters)
{
    // Initialize key scan driver
    key_scan_init();

    // Create a buffer to store key states
    uint8_t key_states[KEY_NUM] = {0};

    // Loop forever
    while (1)
    {
        // Scan keys and update key states
        key_scan_update(key_states);

        // Send key states to RGB LED task and Custom HID task via queues
        xQueueSend(rgb_led_queue, key_states, portMAX_DELAY);
        xQueueSend(custom_hid_queue, key_states, portMAX_DELAY);

        // Delay for 10 ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// RGB LED task function
void rgb_led_task(void *pvParameters)
{
    // Initialize RGB LED driver
    rgb_led_init();

    // Create a buffer to store key states
    uint8_t key_states[KEY_NUM] = {0};

    // Loop forever
    while (1)
    {
        // Receive key states from key scan task via queue
        xQueueReceive(rgb_led_queue, key_states, portMAX_DELAY);

        // Update RGB LED effects based on key states
        rgb_led_update(key_states);
    }
}

// Custom HID task function
void custom_hid_task(void *pvParameters)
{
    // Initialize Custom HID driver
    custom_hid_init();

    // Create a buffer to store key states and HID report data
    uint8_t key_states[KEY_NUM] = {0};
    uint8_t hid_report[8] = {0};

    // Loop forever
    while (1)
    {
        // Receive key states from key scan task via queue
        xQueueReceive(custom_hid_queue, key_states, portMAX_DELAY);

        // Generate HID report data based on key states and HID protocol
        custom_hid_generate_report(key_states, hid_report);

        // Send HID report data to host via USB interface
        custom_hid_send_report(hid_report);

        // Receive HID report data from host via USB interface if any
        if (custom_hid_receive_report(hid_report) == ESP_OK)
        {
            // Process HID report data based on HID protocol and send commands to other tasks via queues if needed
            custom_hid_process_report(hid_report);

            // Process HID report data based on HID protocol and send commands to other tasks via queues if needed
            custom_hid_process_report(hid_report);

            // For example, if the host sends a command to change the RGB LED effect, send it to the RGB LED task via queue
            if (hid_report[0] == 0xAC) // 0xAC is the command code for changing RGB LED effect
            {
                xQueueSend(rgb_led_queue, hid_report, portMAX_DELAY);
            }
        }
    }
}

// NVS config task function
void nvs_config_task(void *pvParameters)
{
    // Initialize NVS config driver
    nvs_config_init();

    // Create a buffer to store NVS config data
    uint8_t nvs_config[NVS_CONFIG_SIZE] = {0};

    // Loop forever
    while (1)
    {
        // Receive NVS config data from Custom HID task via queue if any
        if (xQueueReceive(nvs_config_queue, nvs_config, 0) == pdTRUE)
        {
            // Write NVS config data to NVS partition
            nvs_config_write(nvs_config);
        }

        // Read NVS config data from NVS partition
        nvs_config_read(nvs_config);

        // Send NVS config data to Custom HID task via queue
        xQueueSend(custom_hid_queue, nvs_config, portMAX_DELAY);

        // Delay for 100 ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Main function
void app_main(void)
{
    // Initialize SPI bus for key scan and RGB LED
    spi_bus_config_t spi_bus_cfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .sclk_io_num = SPI_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &spi_bus_cfg, 0));

    // Initialize GPIO for SPI CS
    gpio_pad_select_gpio(SPI_CS);
    gpio_set_direction(SPI_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(SPI_CS, 1);

    // Create queues for inter-task communication
    key_scan_queue = xQueueCreate(10, sizeof(uint8_t) * KEY_NUM);
    rgb_led_queue = xQueueCreate(10, sizeof(uint8_t) * KEY_NUM);
    custom_hid_queue = xQueueCreate(10, sizeof(uint8_t) * 8);
    nvs_config_queue = xQueueCreate(10, sizeof(uint8_t) * NVS_CONFIG_SIZE);

    // Create tasks for key scan, RGB LED, Custom HID and NVS config
    xTaskCreate(key_scan_task, "key_scan_task", 2048, NULL, 5, &key_scan_task_handle);
    xTaskCreate(rgb_led_task, "rgb_led_task", 2048, NULL, 5, &rgb_led_task_handle);
    xTaskCreate(custom_hid_task, "custom_hid_task", 4096, NULL, 5, &custom_hid_task_handle);
    xTaskCreate(nvs_config_task, "nvs_config_task", 2048, NULL, 5, &nvs_config_task_handle);
}