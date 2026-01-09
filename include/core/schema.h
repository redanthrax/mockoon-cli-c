#ifndef SCHEMA_H
#define SCHEMA_H

#include <jansson.h>
#include "mockoon.h"

typedef struct {
  bool valid;
  char **errors;
  int error_count;
} schema_validation_result_t;

schema_validation_result_t* schema_validate_environment(json_t *env_json);
schema_validation_result_t* schema_validate_openapi(json_t *openapi_json);
void schema_validation_result_free(schema_validation_result_t *result);
const char* schema_get_environment_error_message(int index);

#endif /* SCHEMA_H */
