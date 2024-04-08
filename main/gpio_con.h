#ifndef _GPIO_CON
#define _GPIO_CON

#include "esp_log.h"
#include "esp_err.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#define NUM_OF_CHANNELS 16

#define OUT_CH_1 15
#define OUT_CH_2 2
#define OUT_CH_3 4
#define OUT_CH_4 33
#define OUT_CH_5 25
#define OUT_CH_6 26
#define OUT_CH_7 27
#define OUT_CH_8 14
#define OUT_CH_9 12
#define OUT_CH_10 13
#define OUT_CH_11 23
#define OUT_CH_12 22
#define OUT_CH_13 21
#define OUT_CH_14 5
#define OUT_CH_15 17
#define OUT_CH_16 16

typedef struct channel
{
    uint8_t channel;
    bool state;
} io_channel;

esp_err_t agile_gpio_init();
esp_err_t agile_gpio_set(uint8_t channel);
esp_err_t agile_gpio_reset(uint8_t channel);
esp_err_t agile_gpio_set_level(uint8_t channel, bool level);
#endif //_GPIO_CON