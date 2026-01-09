#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "core/schema.h"

static const char* validate_environment_schema(json_t *env_json) {
  if (!env_json || !json_is_object(env_json)) {
    return "Environment must be a JSON object";
  }

  json_t *name = json_object_get(env_json, "name");
  if (!json_is_string(name)) {
    return "Field 'name' is required and must be a string";
  }

  json_t *port = json_object_get(env_json, "port");
  if (!json_is_integer(port)) {
    return "Field 'port' is required and must be an integer";
  }

  int port_val = json_integer_value(port);
  if (port_val < 1 || port_val > 65535) {
    return "Field 'port' must be between 1 and 65535";
  }

  json_t *hostname = json_object_get(env_json, "hostname");
  if (hostname && !json_is_string(hostname)) {
    return "Field 'hostname' must be a string";
  }

  json_t *routes = json_object_get(env_json, "routes");
  if (routes && !json_is_array(routes)) {
    return "Field 'routes' must be an array";
  }

  json_t *cors_rules = json_object_get(env_json, "corsRules");
  if (cors_rules && !json_is_array(cors_rules)) {
    return "Field 'corsRules' must be an array";
  }

  json_t *variables = json_object_get(env_json, "variables");
  if (variables && !json_is_array(variables)) {
    return "Field 'variables' must be an array";
  }

  json_t *data_buckets = json_object_get(env_json, "dataBuckets");
  if (data_buckets && !json_is_array(data_buckets)) {
    return "Field 'dataBuckets' must be an array";
  }

  json_t *proxy_mode = json_object_get(env_json, "proxyMode");
  if (proxy_mode && !json_is_boolean(proxy_mode)) {
    return "Field 'proxyMode' must be a boolean";
  }

  json_t *last_migration = json_object_get(env_json, "lastMigration");
  if (last_migration && !json_is_integer(last_migration)) {
    return "Field 'lastMigration' must be an integer";
  }

  return NULL;
}

static const char* validate_openapi_schema(json_t *openapi_json) {
  if (!openapi_json || !json_is_object(openapi_json)) {
    return "OpenAPI must be a JSON object";
  }

  json_t *swagger = json_object_get(openapi_json, "swagger");
  json_t *openapi = json_object_get(openapi_json, "openapi");
  json_t *info = json_object_get(openapi_json, "info");
  json_t *paths = json_object_get(openapi_json, "paths");

  if (!swagger && !openapi) {
    return "Must contain 'swagger' (v2) or 'openapi' (v3) field";
  }

  if (!info) {
    return "Field 'info' is required";
  }

  if (!json_is_object(info)) {
    return "Field 'info' must be an object";
  }

  if (!paths) {
    return "Field 'paths' is required";
  }

  if (!json_is_object(paths)) {
    return "Field 'paths' must be an object";
  }

  return NULL;
}

schema_validation_result_t* schema_validate_environment(json_t *env_json) {
  schema_validation_result_t *result =
      (schema_validation_result_t *)calloc(1, sizeof(schema_validation_result_t));

  if (!result) {
    return NULL;
  }

  const char *error_msg = validate_environment_schema(env_json);

  if (error_msg) {
    result->valid = false;
    result->error_count = 1;
    result->errors = (char **)malloc(sizeof(char *));
    if (result->errors) {
      result->errors[0] = (char *)error_msg;
    }
  } else {
    result->valid = true;
    result->error_count = 0;
    result->errors = NULL;
  }

  return result;
}

schema_validation_result_t* schema_validate_openapi(json_t *openapi_json) {
  schema_validation_result_t *result =
      (schema_validation_result_t *)calloc(1, sizeof(schema_validation_result_t));

  if (!result) {
    return NULL;
  }

  const char *error_msg = validate_openapi_schema(openapi_json);

  if (error_msg) {
    result->valid = false;
    result->error_count = 1;
    result->errors = (char **)malloc(sizeof(char *));
    if (result->errors) {
      result->errors[0] = (char *)error_msg;
    }
  } else {
    result->valid = true;
    result->error_count = 0;
    result->errors = NULL;
  }

  return result;
}

void schema_validation_result_free(schema_validation_result_t *result) {
  if (!result) {
    return;
  }

  if (result->errors) {
    free(result->errors);
  }

  free(result);
}

const char* schema_get_environment_error_message(int index) {
  static const char *messages[] = {
    "Unknown error",
    "Invalid environment structure",
    "Missing required fields",
    "Invalid field types"
  };

  if (index < 0 || index >= 4) {
    return messages[0];
  }

  return messages[index];
}
