#include "spiffs_con.h"

const static char *APPTAG = "SPIFFS_CON";

/**
 * Initializes the mounted partition for data storage.
 * @return Error status code
 */
esp_err_t agile_spiffs_init()
{
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path = BASEPATH,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_err_t err = esp_vfs_spiffs_register(&spiffs_conf);
    if (err == ESP_OK)
        ESP_LOGI(APPTAG, "Loaded spiffs successfully\n");
    else
        ESP_LOGE(APPTAG, "Loading of spiffs failed\n");
    return err;
}