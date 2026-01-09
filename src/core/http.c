#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "core/http.h"

typedef struct {
  char *data;
  size_t size;
} memory_buffer_t;

static size_t write_callback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  size_t realsize = size * nmemb;
  memory_buffer_t *mem = (memory_buffer_t *)userp;

  char *ptr = realloc(mem->data, mem->size + realsize + 1);
  if (!ptr) {
    return 0;
  }

  mem->data = ptr;
  memcpy(&(mem->data[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->data[mem->size] = 0;

  return realsize;
}

http_result_t http_fetch_url(const char *url, const char *token,
                             http_response_t **response) {
  if (!url || !response) {
    return HTTP_ERROR;
  }

  CURL *curl = curl_easy_init();
  if (!curl) {
    return HTTP_ERROR;
  }

  memory_buffer_t buffer = {0};
  buffer.data = malloc(1);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

  if (token) {
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s",
             token);
    struct curl_slist *headers = curl_slist_append(NULL, auth_header);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  }

  CURLcode res = curl_easy_perform(curl);

  long status_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    free(buffer.data);
    return HTTP_NETWORK_ERROR;
  }

  *response = (http_response_t *)malloc(sizeof(http_response_t));
  if (!*response) {
    free(buffer.data);
    return HTTP_ERROR;
  }

  (*response)->status_code = status_code;
  (*response)->body = buffer.data;
  (*response)->body_len = buffer.size;
  (*response)->content_type = NULL;

  return HTTP_OK;
}

http_result_t http_fetch_cloud_env(const char *env_id, const char *token,
                                   http_response_t **response) {
  if (!env_id || !token || !response) {
    return HTTP_ERROR;
  }

  const char *api_url = "https://api.mockoon.com/environments";
  char url[1024];
  snprintf(url, sizeof(url), "%s/%s", api_url, env_id);

  return http_fetch_url(url, token, response);
}

void http_response_free(http_response_t *response) {
  if (response) {
    if (response->body) {
      free(response->body);
    }
    if (response->content_type) {
      free(response->content_type);
    }
    free(response);
  }
}

const char* http_get_error_message(http_result_t result) {
  switch (result) {
  case HTTP_OK:
    return "OK";
  case HTTP_NOT_FOUND:
    return "Not found";
  case HTTP_UNAUTHORIZED:
    return "Unauthorized";
  case HTTP_FORBIDDEN:
    return "Forbidden";
  case HTTP_ERROR:
    return "HTTP error";
  case HTTP_NETWORK_ERROR:
    return "Network error";
  case HTTP_TIMEOUT:
    return "Request timeout";
  default:
    return "Unknown error";
  }
}
