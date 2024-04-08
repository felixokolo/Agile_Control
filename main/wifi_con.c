#include "wifi_con.h"

static const char *APPTAG = "WIFI CON";

/**
 * Initializes the Wi-Fi peripheral on the ESP-32 to enable wifi connections.
 * @return Error status code
 */
esp_err_t agile_wifi_init()
{
	// Wifi configuration definition
	wifi_config_t wifi_config_s = {
		.ap = {
			.ssid = WIFI_SSID,
			.ssid_len = strlen(WIFI_SSID),
			.password = WIFI_PASS,
			.max_connection = 5,
			.channel = 0,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK,
			.pmf_cfg = {
				.required = false,
			},
		},
	};

	// Initializes the TCP/IP layer of the network for wi-fi use.
	esp_err_t err = esp_netif_init();

	// Registers the default event loop to enable wifi-events dispatch
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_ap();

	// Initialize and configure the wi-fi interface as an Access-Point
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_s));

	// Start the wi-fi Access-Point Interface
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(APPTAG, "WI-Fi setup complete.\nSSID:%s password:%s",
			 wifi_config_s.ap.ssid, wifi_config_s.ap.password);

	return (err);
}
