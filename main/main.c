//假設您的鍵盤電路板有82個按鍵，分別連接到10個移位暫存器的並行輸入端，並且移位暫存器的時鐘端、數據端、片選端和閂鎖端分別連接到ESP32-S3的GPIO 18、19、21和22，那麼您可以參考以下的程式碼：

// 包含ESP-IDF的基本函數和變量
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include <string.h>
#include "rom/ets_sys.h"

// 定義移位暫存器的引腳
#define SCK_PIN 17 // 時鐘線
#define MISO_PIN 18 // 數據線
#define SS_PIN 19 // 片選線
#define LATCH_PIN 20 // 閂鎖線

// 定義按鍵的數量和狀態
#define KEY_COUNT 82 // 按鍵數量
#define KEY_PRESSED 0 // 按下時的邏輯值
#define KEY_RELEASED 1 // 釋放時的邏輯值

// 定義按鍵濾波的參數
#define DEBOUNCE_TIME 40 // 濾波時間，單位毫秒
#define DEBOUNCE_COUNT (DEBOUNCE_TIME / 10) // 濾波次數，每10毫秒掃描一次

// 定義按鍵狀態的變量
uint8_t key_state[KEY_COUNT]; // 當前按鍵狀態
uint8_t key_previous[KEY_COUNT]; // 上一次按鍵狀態
uint16_t key_filter[DEBOUNCE_COUNT]; // 按鍵濾波器

// 額外定義的變量
#define LOW 0
#define HIGH 1
uint16_t readShiftRegister(void);
uint8_t bitRead(uint16_t value, uint8_t bit);
uint16_t bitWrite(uint16_t value, uint8_t bit, uint8_t bitvalue);
void debounceKey(int index);

// 定義SPI設備句柄
spi_device_handle_t spi;

// 掃描按鍵的任務函數
void scan_key_task(void *pvParameters) {
    while (1) {
    // 讀取移位暫存器的串行輸出
    uint16_t data = readShiftRegister();

    // 將串行輸出轉換為按鍵狀態
    for (int i = 0; i < KEY_COUNT; i++) {
        key_state[i] = bitRead(data, i) == KEY_PRESSED ? KEY_PRESSED : KEY_RELEASED;
        if (key_state[i] != key_previous[i]) {
            // 如果按鍵狀態發生變化，則進行濾波處理
            debounceKey(i);
        }
        key_previous[i] = key_state[i];
    }

    // 每10毫秒掃描一次
    vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// 讀取移位暫存器的串行輸出函數
uint16_t readShiftRegister() {
    uint16_t data = 0; // 存儲串行輸出的變量

    // 閂鎖移位暫存器的並行輸入
    gpio_set_level(LATCH_PIN, HIGH);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    gpio_set_level(LATCH_PIN, LOW);

    // 創建一個SPI交易結構體
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = KEY_COUNT;
    t.rx_buffer = &data;

    // 發送SPI交易並等待完成
    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));

    // 返回串行輸出的值
    return data;
}

// 按鍵濾波函數
void debounceKey(int index) {
    // 將當前按鍵狀態寫入濾波器的最低位
    bitWrite(key_filter[0], index, key_state[index]);

    // 檢查濾波器的所有位是否相同
    bool same = true;
    for (int i = 1; i < DEBOUNCE_COUNT; i++) {
    if (key_filter[i] != key_filter[0]) {
        same = false;
        break;
    }
}   

 // 如果濾波器的所有位相同，則認為按鍵狀態穩定，並打印按鍵編號和狀態
 if (same) {
    ESP_LOGI("KEY", "Key %d is %s", index + 1,
            key_state[index] == KEY_PRESSED ? "pressed" : "released");
    }

    // 將濾波器的所有位向左移動一位，並將最高位清零
    for (int i = DEBOUNCE_COUNT - 1; i > 0; i--) {
        key_filter[i] = key_filter[i - 1];
    }
    bitWrite(key_filter[DEBOUNCE_COUNT - 1], index, KEY_RELEASED);
}

// 定義讀取位元的函數
uint8_t bitRead(uint16_t value, uint8_t bit) {
 return (value >> bit) & 0x01;
}

// 定義寫入位元的函數
uint16_t bitWrite(uint16_t value, uint8_t bit, uint8_t bitvalue) {
 if (bitvalue) {
 return value | (1 << bit);
 } else {
 return value & ~(1 << bit);
 }
}

// 初始化函數
void app_main() {
 // 設置串口通信波特率
 esp_log_level_set("*", ESP_LOG_INFO);

 // 設置移位暫存器的引腳模式
 gpio_set_direction(SCK_PIN, GPIO_MODE_OUTPUT);
 gpio_set_direction(MISO_PIN, GPIO_MODE_INPUT);
 gpio_set_direction(SS_PIN, GPIO_MODE_OUTPUT);
 gpio_set_direction(LATCH_PIN, GPIO_MODE_OUTPUT);

 // 設置 MISO_PIN 的上拉電阻
 gpio_set_pull_mode(MISO_PIN, GPIO_PULLUP_ONLY);

 // 初始化移位暫存器的引腳狀態
 gpio_set_level(SCK_PIN, LOW);
 gpio_set_level(SS_PIN, HIGH);
 gpio_set_level(LATCH_PIN, LOW);

 // 初始化按鍵狀態和濾波器
 for (int i = 0; i < KEY_COUNT; i++) {
    key_state[i] = KEY_RELEASED;
    key_previous[i] = KEY_RELEASED;
 }
 for (int i = 0; i < DEBOUNCE_COUNT; i++) {
    key_filter[i] = 0;
 }

 // 初始化SPI設備配置結構體
 spi_bus_config_t buscfg = {
    .miso_io_num = MISO_PIN,
    .mosi_io_num = -1,
    .sclk_io_num = SCK_PIN,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = KEY_COUNT * 8,
    .flags = SPICOMMON_BUSFLAG_MASTER,
    .intr_flags = ESP_INTR_FLAG_IRAM,
 };

 spi_device_interface_config_t devcfg = {
    .clock_speed_hz = 16 * 1000 * 1000, // 時鐘頻率
    .mode = 0, // SPI模式
    .spics_io_num = SS_PIN, // 片選引腳
    .queue_size = 1, // 佇列大小
    .flags = SPI_DEVICE_HALFDUPLEX, // 半雙工模式
 };

 // 初始化SPI總線和設備
 ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, 0));
 ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &devcfg, &spi));

 // 創建一個任務來掃描按鍵
 xTaskCreate(scan_key_task, "scan_key_task", 2048, NULL, 5, NULL);

}
