#include "hex_parser.h"
#include "avr_flash.h"

static const char *TAG = "esp_avr_flash";

uint8_t page[PAGE_SIZE_MAX];
int block_count = 0;

void flashTask(char *filepath)
{
    logI(TAG, "%s", "Writing file to page");
    ESP_ERROR_CHECK(hexFileParser(filepath, page, &block_count));

    logI(TAG, "%s", "Writing code to AVR memory");
    ESP_ERROR_CHECK(writeTask(page, block_count));

    logI(TAG, "%s", "Reading Memory");
    ESP_ERROR_CHECK(readTask(page, block_count));

    logI(TAG, "%s", "Ending Connection");
    endConn();
}

void initTask(void)
{
    initUART();
    initGPIO();
    initSPIFFS();
    setupDevice();
}

void read_uart(void)
{
    time_t start = time(0);
    while (time(0) - start < 10)
    {
        char buf;
        int rc = uart_read_bytes(UART_NUM_2, &buf, 1, 0);
        if (rc == 1)
            printf("%c", buf);
        vTaskDelay(1);
    }

}

void app_main(void)
{
    initTask();

    while(1)
    {
        flashTask("/spiffs/evse1.firmware.hex");
        read_uart();
        flashTask("/spiffs/evse2.firmware.hex");
        read_uart();
    }
}
