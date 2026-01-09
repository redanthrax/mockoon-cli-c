#ifndef JSON_H
#define JSON_H

#include <jansson.h>
#include "mockoon.h"

typedef enum {
  JSON_OK,
  JSON_PARSE_ERROR,
  JSON_INVALID_FORMAT,
  JSON_MISSING_FIELD
} json_result_t;

json_result_t json_parse_string(const char *json_str, json_t **result);
json_result_t json_parse_file(const char *filepath, json_t **result);
json_result_t json_to_string(json_t *obj, bool prettify, char **result);
json_result_t json_load_environment(json_t *obj, environment_t **env);
json_result_t json_validate_environment(json_t *obj);
void json_free_environment(environment_t *env);
char* json_get_error_message(json_result_t result);
json_t* json_environment_to_json(environment_t *env);

#endif /* JSON_H */
