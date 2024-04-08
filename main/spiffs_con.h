#ifndef _SPIFFS_CON
#define _SPIFFS_CON

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_err.h"

#define BASEPATH "/web"

esp_err_t agile_spiffs_init();

#endif // _SPIFFS_CON
