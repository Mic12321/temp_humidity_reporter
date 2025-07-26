#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "config.h"
#include "DHT22.h"

#define DHT_PIN 15

bool connect_wifi() {
    printf("Connecting to Wi-Fi...\n");

    cyw43_arch_enable_sta_mode();  
    int result = cyw43_arch_wifi_connect_timeout_ms(
        WIFI_SSID,
        WIFI_PASSWORD,
        CYW43_AUTH_WPA2_AES_PSK,
        10000
    );

    if (result) {
        printf("Wi-Fi connection failed. Error: %d\n", result);
        return false;
    } else {
        printf("Connected to Wi-Fi!\n");
        return true;
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);  // Let USB serial settle

    if (cyw43_arch_init()) {
        printf("Wi-Fi initialization failed!\n");
        return -1;
    }

    dht22_init(DHT_PIN);  // Initialize DHT22 GPIO

    float temp = 0.0f, hum = 0.0f;

    if (!connect_wifi()) {
        printf("Initial Wi-Fi connection failed. Retrying in 10 seconds...\n");
        sleep_ms(10000);
    }

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(500);
    gpio_put(DHT_PIN, 1);

    while (true) {
        // Check Wi-Fi connection
        if (!cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA)) {
            printf("Wi-Fi disconnected! Reconnecting...\n");
            while (!connect_wifi()) {
                printf("Retrying in 5 seconds...\n");
                sleep_ms(5000);
            }
        }

        // Read DHT22
        if (read_dht22(DHT_PIN, &temp, &hum)) {
            printf("Temperature: %.1f °C  Humidity: %.1f %%\n", temp, hum);
        } else {
            printf("Failed to read from DHT22\n");
        }

        printf("Sleeping for 5s...\n");
        sleep_ms(5000);
    }

    return 0;
}
