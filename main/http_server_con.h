#ifndef _HTTP_SERVER_CON
#define _HTTP_SERVER_CON

#include <esp_http_server.h>
#include "esp_log.h"
#include <string.h>
#include "mdns.h"
#include "math.h"
#include <ctype.h>

#include "spiffs_con.h"
#include "gpio_con.h"

httpd_handle_t agile_http_server_init();
esp_err_t homepage_handler(httpd_req_t *req);
esp_err_t register_hompepage(httpd_handle_t *server);
esp_err_t PUT_handler(httpd_req_t *req);
esp_err_t register_PUT_hompepage(httpd_handle_t *server);
esp_err_t mdns_service_init(char const *hostname,
							char const *default_instance_name);
esp_err_t get_filetype(const char *filename, char *filetype);
esp_err_t agile_execute(char *buf);
int get_next_number(char *src_ptr, char *dst_ptr);

#endif
