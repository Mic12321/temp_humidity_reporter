#include "pico/stdlib.h"
#include <stdio.h>
#include "dht22.h"

void dht22_init(uint gpio_pin) {
    gpio_init(gpio_pin);
    gpio_set_dir(gpio_pin, GPIO_OUT);
    gpio_put(gpio_pin, 1);
}

bool read_dht22(uint gpio_pin, float* temperature, float* humidity) {
    uint8_t data[5] = {0};

    printf("[DHT] Starting read...\n");

    gpio_pull_up(gpio_pin);

    // Step 1: Send start signal
    gpio_set_dir(gpio_pin, GPIO_OUT);
    gpio_put(gpio_pin, 0);
    sleep_ms(20);  // At least 1ms for DHT22; 20ms is safe
    gpio_put(gpio_pin, 1);
    sleep_us(30);
    gpio_set_dir(gpio_pin, GPIO_IN);

    // Step 2: Wait for response
    uint32_t timeout = 0;
    while (gpio_get(gpio_pin)) {
        if (++timeout > 10000) {
            printf("[DHT] Timeout on first LOW\n");
            return false;
        }
    }
    timeout = 0;
    while (!gpio_get(gpio_pin)) {
        if (++timeout > 10000) {
            printf("[DHT] Timeout on first HIGH\n");
            return false;
        }
    }
    timeout = 0;
    while (gpio_get(gpio_pin)) {
        if (++timeout > 10000) {
            printf("[DHT] Timeout before data\n");
            return false;
        }
    }

    // Step 3: Read 40 bits
    for (int i = 0; i < 40; ++i) {
        while (!gpio_get(gpio_pin));
        sleep_us(28);
        if (gpio_get(gpio_pin))
            data[i / 8] |= (1 << (7 - (i % 8)));

        timeout = 0;
        while (gpio_get(gpio_pin)) {
            if (++timeout > 10000) {
                printf("[DHT] Timeout during bit read\n");
                return false;
            }
        }
    }

    // Step 4: Verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (data[4] != checksum) {
        printf("[DHT] Checksum failed: expected %u, got %u\n", checksum, data[4]);
        return false;
    }

    // Step 5: Convert to values
    *humidity = ((data[0] << 8) | data[1]) * 0.1f;
    int16_t raw_temp = (data[2] << 8) | data[3];
    if (raw_temp & 0x8000) raw_temp = -(raw_temp & 0x7FFF);
    *temperature = raw_temp * 0.1f;

    printf("[DHT] Read success! T: %.1f°C, H: %.1f%%\n", *temperature, *humidity);
    return true;
}
