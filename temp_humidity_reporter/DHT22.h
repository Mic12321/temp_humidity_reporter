#ifndef DHT22_H
#define DHT22_H

#include "pico/stdlib.h"

void dht22_init(uint gpio_pin);
bool read_dht22(uint gpio_pin, float* temperature, float* humidity);

#endif