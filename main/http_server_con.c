#include "http_server_con.h"

const static char *HOSTNAME = "agile-home";
const static char *DEFAULT_NAME = "Agile Home Automation";

const static char *APPTAG = "HTTP_SERVER_CON";

/**
 * Configures and starts an HTTP server on the ESP-32.
 * @return Server handle
 */
httpd_handle_t agile_http_server_init()
{
	// Initialize and start the DNS server for hostname resolution
	mdns_service_init(HOSTNAME, DEFAULT_NAME);

	// HTTP Server configuration
	static httpd_handle_t server = NULL;
	httpd_config_t http_config = HTTPD_DEFAULT_CONFIG();
	http_config.max_uri_handlers = 10; // Maximum possible uri handlers
	http_config.lru_purge_enable = true;
	// Enable the use of wildcards in uri
	http_config.uri_match_fn = httpd_uri_match_wildcard;
	esp_err_t err = httpd_start(&server, &http_config);

	// Error handling
	if (err == ESP_OK)
	{
		ESP_LOGI(APPTAG, "HTTP Server started");

		// Register uri handlers
		register_hompepage(&server);
		register_PUT_hompepage(&server);
	}
	else
		ESP_LOGE(APPTAG, "Error satrting HTTP Server");

	// Initialize and setup file system to access web files
	err = agile_spiffs_init();

	return server;
}

/**
 * HTTP GET Handler for home page route.
 * @param [in] req Request handle pointer.
 * @return Error satus code.
 */

esp_err_t homepage_handler(httpd_req_t *req)
{
	char recv_buf[100];

	ESP_LOGI(APPTAG, "Requesting %s", req->uri);

	char filename[30];
	char filetype[10];

	esp_err_t err = ESP_FAIL;

	// Get the basepath to file system and requested file path
	strcpy(filename, BASEPATH);

	// Default to index.html if filename missing else get filename
	if (strlen(req->uri) == 1)
		strcat(filename, "/index.html");
	else
		strcat(filename, req->uri);

	/**
	 * Determine file type and set the corresponding content-type in
	 * HTTP response.
	 */
	get_filetype(filename, filetype);

	if (strcmp(filetype, "css") == 0)
		httpd_resp_set_type(req, "text/css");

	httpd_resp_set_status(req, HTTPD_200);

	// Get access to file system and read requested file
	FILE *f = fopen(filename, "r");

	// If file is missing send 404 response
	if (f == NULL)
	{
		ESP_LOGE(APPTAG, "Error opening file");
		err = httpd_resp_send_404(req);
		return ESP_FAIL;
	}
	else
	{
		ESP_LOGI(APPTAG, "Opened file successfully");
	}

	// Send file using chunk encoding
	for (char *s = fgets(recv_buf, sizeof(recv_buf), f); s != NULL;
		 s = fgets(recv_buf, sizeof(recv_buf), f))
	{
		err = httpd_resp_send_chunk(req, recv_buf, strlen(recv_buf));
		if (err != ESP_OK)
		{
			ESP_LOGE(APPTAG, "Error sending response");
		}
	}
	// End chunk sending
	err = httpd_resp_send_chunk(req, recv_buf, 0);

	// Release file
	fclose(f);
	return err;
}

/**
 * Registers homepage GET handler to server.
 * @param [in] server Server handle pointer.
 * @return Error satus code.
 */
esp_err_t register_hompepage(httpd_handle_t *server)
{
	// Define URI structure
	httpd_uri_t homepage_uri = {
		.uri = "/*",
		.method = HTTP_GET,
		.handler = homepage_handler,
		.user_ctx = NULL};

	// Register uri using defined structure
	esp_err_t err = httpd_register_uri_handler(*server, &homepage_uri);
	if (err != ESP_OK)
	{
		ESP_LOGE(APPTAG, "Error registering uri route %d", err);
		return err;
	}

	ESP_LOGI(APPTAG, "Registered homepage uri route");
	return err;
}

/**
 * PUT handler to process PUT requests.
 * Used for setting the states of the channels.
 * Required data is json and the following format
 * Array of channel properties
 * [{"channel": <channel number>, "state": <1 - ON or 0 - OFF>}, ...]
 * both channel properties (channel and state) are of int type.
 * @param [in] req Request handle pointer.
 * @return Error satus code.
 */
esp_err_t PUT_handler(httpd_req_t *req)
{
	char buf[1024];
	int content_len = req->content_len;
	// esp_err_t err = ESP_FAIL;
	if (content_len > 0)
		do
		{
			int read_size = read_size = httpd_req_recv(req, buf, fmin(content_len, sizeof(buf) - 1));
			buf[read_size] = 0;
			agile_execute(buf);
			content_len -= read_size;
		} while (content_len > 0);

	httpd_resp_set_status(req, HTTPD_200);
	httpd_resp_send(req, NULL, 0);
	return ESP_OK;
}

/**
 * Registers homepage PUT handler to server.
 * @param [in] server Server handle pointer.
 * @return Error satus code.
 */
esp_err_t register_PUT_hompepage(httpd_handle_t *server)
{
	esp_err_t err = ESP_FAIL;
	httpd_uri_t home_PUT_uri = {
		.uri = "/*",
		.method = HTTP_PUT,
		.handler = PUT_handler,
		.user_ctx = NULL};
	err = httpd_register_uri_handler(*server, &home_PUT_uri);
	if (err != ESP_OK)
	{
		ESP_LOGE(APPTAG, "Error registering PUT uri route %d", err);
		return err;
	}

	ESP_LOGI(APPTAG, "Registered home PUT uri route");
	return err;
}

/**
 * Configure mdns service with hostname.
 * @param [in] hostname Desired hostname.
 * @param [in] default_instance_name Desired
 */
esp_err_t mdns_service_init(char const *hostname,
							char const *default_instance_name)
{
	esp_err_t err = mdns_init();
	if (err != ESP_OK)
	{
		ESP_LOGE(APPTAG, "Failed to initialize MDNS service %d", err);
		return err;
	}

	ESP_LOGI(APPTAG, "Initialized MDNS service");
	// Set hostname
	err = mdns_hostname_set(hostname);
	mdns_instance_name_set(default_instance_name);
	return err;
}

/**
 * Extracts filename from a given uri.
 * @param [in] uri Pointer to URI string containing the desired filename.
 * @param [in] filename Pointer to buffer to copy filename into.
 * @return Error status code.
 */
esp_err_t get_filetype(const char *filename, char *filetype)
{
	// Search for . in filename to extract the file extension
	char *ptr = strchr(filename, '.');
	ptr++;
	// c Copy file extension to filetype
	strcpy(filetype, ptr);
	return ESP_OK;
}

esp_err_t agile_execute(char *buf)
{
	char *ch = "channel";
	char *st = "state";
	int channel = 0;
	int state = 0;
	char tmp[10];

	// Error handling
	if (strlen(buf) <= 0)
		return ESP_FAIL;

	// Search for channel property
	char *ptr = strstr(buf, ch);
	// If found
	if (ptr != NULL)
		do
		{
			// reset the temp buffer
			tmp[0] = 0;
			ptr += strlen(ch);

			// Save in the channel property value
			channel = get_next_number(ptr, tmp);

			// Error handling
			if (ptr == NULL)
				return ESP_FAIL;

			// Search for state property name
			ptr = strstr(ptr, st);

			// Reset the temp buffer
			tmp[0] = 0;
			ptr += strlen(st);
			// Save in the state property value
			state = get_next_number(ptr, tmp);

			agile_gpio_set_level(channel, state);

			ptr = strstr(ptr, ch);
		} while (ptr != NULL);
	return ESP_OK;
}

/**
 * Get the next found number in scr_ptr.
 * @param [in] src_ptr Pointer to source
 * @param [in] dst_ptr Pointer to destination
 * @return Found number or -1 if nothing is found.
 */
int get_next_number(char *src_ptr, char *dst_ptr)
{
	*dst_ptr = 0;
	// Find the next digit after the channel property name
	while (src_ptr != NULL && !isdigit((int)(*src_ptr)))
		src_ptr++;

	// Exit if none is found
	if (src_ptr == NULL)
		return ESP_FAIL;

	// Get the number afer the channel property name
	while (src_ptr != NULL && isdigit((int)(*src_ptr)))
	{
		// Copy each found digit into the temp buffer
		strncat(dst_ptr, src_ptr, 1);
		src_ptr++;
	}

	return atoi(dst_ptr);
}