idf_component_register(SRCS "agile_control_main.c" "wifi_con.c" "http_server_con.c" "spiffs_con.c"
                            "gpio_con.c"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(web ../frontend FLASH_IN_PROJECT)