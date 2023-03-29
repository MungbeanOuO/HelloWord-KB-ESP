#include "key_scan.h"

// Define the SPI device handle for key scan
spi_device_handle_t key_scan_spi;

// Initialize key scan driver
void key_scan_init(void)
{
    // Configure the SPI device for key scan
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
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    // Add the SPI device for key scan to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &spi_dev_cfg, &key_scan_spi));
}

// Scan keys and update key states
void key_scan_update(uint8_t *key_states)
{
    // Create a buffer to store key scan data
    uint8_t key_scan_data[KEY_NUM / 8] = {0};

    // Create a SPI transaction for key scan
    spi_transaction_t spi_trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = KEY_NUM,
        .rxlength = KEY_NUM,
        .user = NULL,
        .tx_buffer = NULL,
        .rx_buffer = key_scan_data,
    };

    // Perform the SPI transaction for key scan
    ESP_ERROR_CHECK(spi_device_transmit(key_scan_spi, &spi_trans));

    // Invert the key scan data, since the shift registers use active low logic
    for (int i = 0; i < KEY_NUM / 8; i++)
    {
        key_scan_data[i] = ~key_scan_data[i];
    }

    // Update the key states by using XOR operator, since only the changed keys need to be sent
    for (int i = 0; i < KEY_NUM; i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        uint8_t bit_mask = 1 << bit_index;
        key_states[i] ^= (key_scan_data[byte_index] & bit_mask) >> bit_index;
    }
}