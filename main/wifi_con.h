#ifndef _WIFI_CON
#define _WIFI_CON

#include "esp_wifi.h"
#include "esp_log.h"
#include <string.h>

#define WIFI_SSID "Agile Home"
#define WIFI_PASS "12345678"

esp_err_t agile_wifi_init();

#endif
