#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "config.h"


bool connect_wifi() {
    printf("Connecting to Wi-Fi...\n");

    cyw43_arch_enable_sta_mode();  
    int result = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000);

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

    if (cyw43_arch_init()) {
        printf("Wi-Fi initialization failed!\n");
        return -1;
    }

    if (!connect_wifi()) {
        printf("Initial Wi-Fi connection failed. Retrying in 10 seconds...\n");
    }

    while (true) {

        if (!cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA)) {
            printf("Wi-Fi disconnected! Reconnecting...\n");

            // Try to reconnect
            while (!connect_wifi()) {
                printf("Retrying in 5 seconds...\n");
                sleep_ms(5000);
            }
        } else {
            printf("Wi-Fi connected and stable.\n");
        }

        sleep_ms(5000);
    }

    return 0;
}