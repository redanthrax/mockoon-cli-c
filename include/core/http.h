#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>

typedef enum {
  HTTP_OK,
  HTTP_NOT_FOUND,
  HTTP_UNAUTHORIZED,
  HTTP_FORBIDDEN,
  HTTP_ERROR,
  HTTP_NETWORK_ERROR,
  HTTP_TIMEOUT
} http_result_t;

typedef struct {
  long status_code;
  char *body;
  size_t body_len;
  char *content_type;
} http_response_t;

http_result_t http_fetch_url(const char *url, const char *token,
                             http_response_t **response);
http_result_t http_fetch_cloud_env(const char *env_id, const char *token,
                                   http_response_t **response);
void http_response_free(http_response_t *response);
const char* http_get_error_message(http_result_t result);

#endif /* HTTP_H */
