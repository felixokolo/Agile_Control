#include "includes.h"

void app_main(void)
{
	nvs_flash_init();
	agile_wifi_init();
	agile_http_server_init();
	agile_gpio_init();
	while (1)
	{
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
