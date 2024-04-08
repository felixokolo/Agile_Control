#include "gpio_con.h"

static const char *APPTAG = "GPIO CON";

static io_channel CHANNELS[] = {
    {.channel = OUT_CH_1, .state = false},
    {.channel = OUT_CH_2, .state = false},
    {.channel = OUT_CH_3, .state = false},
    {.channel = OUT_CH_4, .state = false},
    {.channel = OUT_CH_5, .state = false},
    {.channel = OUT_CH_6, .state = false},
    {.channel = OUT_CH_7, .state = false},
    {.channel = OUT_CH_8, .state = false},
    {.channel = OUT_CH_9, .state = false},
    {.channel = OUT_CH_10, .state = false},
    {.channel = OUT_CH_11, .state = false},
    {.channel = OUT_CH_12, .state = false},
    {.channel = OUT_CH_13, .state = false},
    {.channel = OUT_CH_14, .state = false},
    {.channel = OUT_CH_15, .state = false},
    {.channel = OUT_CH_16, .state = false}};

/**
 * Initializes all the gpio on the board.
 * Initializes the states of the gpios using the state in channel
 * if available else channels are filled with new channel states.
 * @param [inout] channels Pointer to array of io_channel holding the
 * states of each channel.
 * @return Error status code
 */
esp_err_t agile_gpio_init()
{
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    uint64_t bit_mask = 0;
    for (int channel = 0; channel < NUM_OF_CHANNELS; channel++)
    {
        uint64_t mask = 0X01;
        esp_rom_gpio_pad_select_gpio(CHANNELS[channel].channel);
        bit_mask = bit_mask | (mask << (CHANNELS[channel].channel));
    }
    io_conf.pin_bit_mask = bit_mask;
    gpio_config(&io_conf);

    /*for (int channel = 1; channel <= NUM_OF_CHANNELS; channel++)
    {
        agile_gpio_set(channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }*/

    return ESP_OK;
}

/**
 * Sets the level of a gpio pin.
 * @param [in] channel Channel to set.
 * @return Error status code.
 */
esp_err_t agile_gpio_set(uint8_t channel)
{
    if (channel == 0 || channel > NUM_OF_CHANNELS)
    {
        ESP_LOGE(APPTAG, "Invalid channel");
        return ESP_FAIL;
    }

    channel -= 1;

    CHANNELS[channel].state = true;
    esp_err_t err = gpio_set_level(CHANNELS[channel].channel, true);
    return err;
}

/**
 * Clears the level of a gpio pin.
 * @param [in] channel Channel to clear.
 * @return Error status code.
 */
esp_err_t agile_gpio_reset(uint8_t channel)
{
    if (channel == 0 || channel > NUM_OF_CHANNELS)
    {
        ESP_LOGE(APPTAG, "Invalid channel");
        return ESP_FAIL;
    }

    channel -= 1;

    CHANNELS[channel].state = false;
    esp_err_t err = gpio_set_level(CHANNELS[channel].channel, false);
    return err;
}

/**
 * Sets the level of a gpio pin.
 * @param [in] channel Channel to set.
 * @param [in] level Level to set channel.
 * @return Error status code.
 */
esp_err_t agile_gpio_set_level(uint8_t channel, bool level)
{
    if (channel == 0 || channel > NUM_OF_CHANNELS)
    {
        ESP_LOGE(APPTAG, "Invalid channel %d", channel);
        return ESP_FAIL;
    }

    channel -= 1;

    CHANNELS[channel].state = true;
    esp_err_t err = gpio_set_level(CHANNELS[channel].channel, level);

    ESP_LOGI(APPTAG, "Channel %d set to %d", channel + 1, level);
    return err;
}