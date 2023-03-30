# MungbeanOuO/HelloWord-KB-ESP

這個專案目標是在esp32-s3上重現[HelloWord-Keyboard-fw](https://github.com/peng-zhihui/HelloWord-Keyboard/tree/main/2.Firmware/HelloWord-Keyboard-fw)的主要功能，並增加8000hz 輪詢率、2.4Ghz無線、藍芽、有線三模式並使用按鍵組合進行切換模式。

## 本repo大部分程式碼都是使用Bing AI生成的，所以大部分功能還有待調整修正

## 目錄

- [需求](#需求)
- [安裝](#安裝)
- [配置](#配置)
- [貢獻](#貢獻)
- [授權](#授權)

## 需求

這個專案需要以下的硬體和軟體：

- ESP32-S3開發板或模組（開發版使用ESP32-S3-DevKitC-1 v1.1，量產版使用ESP32-S3-WROOM-1-N16R8）
- HelloWord-Keyboard-fw的硬體設計（鍵盤、電池、充電線等）
- esp-idf開發環境
- USB轉TTL串口模組（用於燒錄固件，開發版不需要此需求）

## 安裝

以下是如何在你的本地環境上安裝和運行這個專案的步驟：

1. 下載或克隆這個專案的代碼庫
   ```
   git clone https://github.com/MungbeanOuO/HelloWord-KB-ESP.git
   ```
2. 安裝esp-idf開發環境，並設置好路徑和工具鏈，詳細步驟請參考[官方文檔](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html)
補充說明：建議使用手動安裝ESP-IDF Tools 後再使用VS code安裝esp-idf插件，之後ctrl+shift+p輸入"ESP-IDF: Configure ESP-IDF extension"點擊"advanced"進行安裝

*3,4點只有量產版需要操作*
3. 連接ESP32-S3開發板或模組到電腦，並使用USB轉TTL串口模組連接到ESP32-S3的UART0端口（GPIO43和GPIO44）
4. 在專案目錄下執行以下命令，選擇正確的串口號和波特率，並燒錄固件到ESP32-S3
   ```
   idf.py -p PORT -b BAUD flash
   ```
5. 燒錄完成後，重新啟動ESP32-S3，並享受你的HelloWord-KB-ESP吧！

## 配置

你可以通過修改專案目錄下的~config.h~文件來配置一些參數，例如：

這邊可還沒生出來
//- 按鍵映射表
//- 按鍵組合切換模式的方式
//- 藍芽設備名稱
//- 電池電壓檢測閾值
//- LED指示燈亮度

修改完配置後，記得重新編譯和燒錄固件。

## 貢獻

歡迎任何人對這個專案提出問題、建議或改進。

## 授權

本專案跟瀚文使用相同的GPL-3授權。

[https://github.com/peng-zhihui/HelloWord-Keyboard](https://github.com/peng-zhihui/HelloWord-Keyboard)

