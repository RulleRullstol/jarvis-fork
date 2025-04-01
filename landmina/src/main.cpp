#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_WS  15
#define I2S_SD  32
#define I2S_SCK 14

#define SAMPLE_RATE 16000
#define I2S_PORT I2S_NUM_0
#define BUFFER_SIZE 1024

void setup() {
    Serial.begin(115200);

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = BUFFER_SIZE,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1, // Not used
        .data_in_num = I2S_SD
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
}

void loop() {
    int32_t buffer[BUFFER_SIZE];
    size_t bytes_read;

    i2s_read(I2S_PORT, buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

    if (bytes_read > 0) {
        Serial.write((uint8_t*)buffer, bytes_read);
    }
}
