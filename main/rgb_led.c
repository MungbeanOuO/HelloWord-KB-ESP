#include "rgb_led.h"

// Define the SPI device handle for RGB LED
spi_device_handle_t rgb_led_spi;

// Define the RGB LED data buffer and its size
static uint8_t rgb_led_data[RGB_LED_NUM * 4] = {0};
#define RGB_LED_DATA_SIZE (sizeof(rgb_led_data) / sizeof(rgb_led_data[0]))

// Define the RGB LED effect type and its parameters
static uint8_t rgb_led_effect_type = RGB_LED_EFFECT_NONE;
static uint8_t rgb_led_effect_param[3] = {0}; // For brightness, speed and color

// Initialize RGB LED driver
void rgb_led_init(void)
{
    // Configure the SPI device for RGB LED
    spi_device_interface_config_t spi_dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 10000000, // Increase the clock speed to 10 MHz
        .input_delay_ns = 0,
        .spics_io_num = SPI_CS,
        .flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    // Add the SPI device for RGB LED to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &spi_dev_cfg, &rgb_led_spi));
}

// Update RGB LED effects based on key states
void rgb_led_update(uint8_t *key_states)
{
    // Clear the RGB LED data buffer
    memset(rgb_led_data, 0, RGB_LED_DATA_SIZE);

    // Set the RGB LED data buffer according to the key states and the RGB LED effect type
    switch (rgb_led_effect_type)
    {
    case RGB_LED_EFFECT_NONE:
        // No effect, just turn on the pressed keys with white color
        for (int i = 0; i < KEY_NUM; i++)
        {
            if (key_states[i] == 1) // Key is pressed
            {
                int led_index = i * 4;
                rgb_led_data[led_index] = 0xE0 | rgb_led_effect_param[0]; // Brightness
                rgb_led_data[led_index + 1] = 0xFF; // Blue
                rgb_led_data[led_index + 2] = 0xFF; // Green
                rgb_led_data[led_index + 3] = 0xFF; // Red
            }
        }
        break;
    case RGB_LED_EFFECT_BREATH:
        // Breathing effect, change the brightness of all keys with a sine wave
        for (int i = 0; i < KEY_NUM; i++)
        {
            int led_index = i * 4;
            rgb_led_data[led_index] = 0xE0 | (uint8_t)(sin(xTaskGetTickCount() * M_PI / (RGB_LED_EFFECT_BREATH_PERIOD / rgb_led_effect_param[1])) * rgb_led_effect_param[0]); // Brightness
            rgb_led_data[led_index + 1] = rgb_led_effect_param[2]; // Blue
            rgb_led_data[led_index + 2] = rgb_led_effect_param[2]; // Green
            rgb_led_data[led_index + 3] = rgb_led_effect_param[2]; // Red
        }
        break;
    case RGB_LED_EFFECT_RAINBOW:
        // Rainbow effect, change the color of all keys with a hue wheel
        for (int i = 0; i < KEY_NUM; i++)
        {
            int led_index = i * 4;
            rgb_led_data[led_index] = 0xE0 | rgb_led_effect_param[0]; // Brightness
            uint8_t hue = (xTaskGetTickCount() / (RGB_LED_EFFECT_RAINBOW_PERIOD / rgb_led_effect_param[1]) + i * 255 / KEY_NUM) % 255; // Hue
            uint8_t r, g, b;
            hue_to_rgb(hue, &r, &g, &b); // Convert hue to RGB values
            rgb_led_data[led_index + 1] = b;
            rgb_led_data[led_index + 2] = g;
            rgb_led_data[led_index + 3] = r;
        }
        break;
    default:
        // Unknown effect type, do nothing
        break;
    }

    // Create a SPI transaction for RGB LED
    spi_transaction_t spi_trans = {
        .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
        .cmd = 0,
        .addr = 0,
        .length = RGB_LED_DATA_SIZE * 8,
        .rxlength = RGB_LED_DATA_SIZE * 8,
        .user = NULL,
        .tx_buffer = rgb_led_data,
        .rx_buffer = NULL,
    };

    // Perform the SPI transaction for RGB LED
    ESP_ERROR_CHECK(spi_device_transmit(rgb_led_spi, &spi_trans));
}

// Change RGB LED effect type based on HID command
void rgb_led_change_effect(uint8_t *hid_report)
{
    // Parse the HID command and update the RGB LED effect type and parameters
    rgb_led_effect_type = hid_report[1]; // Effect type
    rgb_led_effect_param[0] = hid_report[2]; // Brightness
    rgb_led_effect_param[1] = hid_report[3]; // Speed
    rgb_led_effect_param[2] = hid_report[4]; // Color
}

// Convert hue to RGB values
void hue_to_rgb(uint8_t hue, uint8_t *r, uint8_t *g, uint8_t *b)
{
    // Use a simple algorithm to map hue to RGB values
    uint8_t x = (hue / 60) % 6;
    uint8_t c = 255;
    uint8_t m = hue % 60;

    switch (x)
    {
    case 0:
        *r = c;
        *g = m * 255 / 60;
        *b = 0;
        break;
    case 1:
        *r = (60 - m) * 255 / 60;
        *g = c;
        *b = 0;
        break;
    case 2:
        *r = 0;
        *g = c;
        *b = m * 255 / 60;
        break;
    case 3:
        *r = 0;
        *g = (60 - m) * 255 / 60;
        *b = c;
        break;
    case 4:
        *r = m * 255 / 60;
        *g = 0;
        *b = c;
        break;
    case 5:
        *r = c;
        *g = 0;
        *b = (60 - m) * 255 / 60;
        break;
    default:
        // Unknown hue value, do nothing
        break;
    }
}