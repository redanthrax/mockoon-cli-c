#ifndef OPENAPI_H
#define OPENAPI_H

#include <jansson.h>
#include "mockoon.h"

typedef enum {
  OPENAPI_OK,
  OPENAPI_INVALID_VERSION,
  OPENAPI_PARSE_ERROR,
  OPENAPI_CONVERSION_ERROR,
  OPENAPI_UNSUPPORTED_FEATURE
} openapi_result_t;

typedef enum {
  OPENAPI_V2,
  OPENAPI_V3,
  OPENAPI_UNKNOWN
} openapi_version_t;

openapi_version_t openapi_detect_version(json_t *json);
openapi_result_t openapi_convert_to_environment(json_t *openapi_json,
                                               environment_t **env);
openapi_result_t openapi_convert_from_environment(environment_t *env,
                                                  const char *format,
                                                  char **result);
const char* openapi_get_error_message(openapi_result_t result);

#endif /* OPENAPI_H */
