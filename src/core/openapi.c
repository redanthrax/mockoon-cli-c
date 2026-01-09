#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <jansson.h>
#include "core/openapi.h"
#include "core/json.h"

openapi_version_t openapi_detect_version(json_t *json) {
  if (!json || !json_is_object(json)) {
    return OPENAPI_UNKNOWN;
  }

  json_t *swagger = json_object_get(json, "swagger");
  if (json_is_string(swagger)) {
    return OPENAPI_V2;
  }

  json_t *openapi = json_object_get(json, "openapi");
  if (json_is_string(openapi)) {
    const char *version = json_string_value(openapi);
    if (version && version[0] == '3') {
      return OPENAPI_V3;
    }
  }

  return OPENAPI_UNKNOWN;
}

static json_t* create_mockoon_route_from_openapi_path(
    const char *path, const char *method, json_t *operation) {
  json_t *route = json_object();

  json_object_set_new(route, "uuid", json_string(path));
  json_object_set_new(route, "type", json_string("http"));
  json_object_set_new(route, "documentation", json_string(""));
  json_object_set_new(route, "method", json_string(method));
  json_object_set_new(route, "endpoint", json_string(path));
  json_object_set_new(route, "enabled", json_true());
  json_object_set_new(route, "responseMode", json_null());

  json_t *responses = json_object_get(operation, "responses");
  int status_code = 200;

  if (json_is_object(responses)) {
    const char *key = NULL;
    json_t *resp = NULL;

    json_object_foreach(responses, key, resp) {
      if (key) {
        if (strcmp(key, "2XX") == 0 || strcmp(key, "200") == 0 ||
            strcmp(key, "201") == 0) {
          status_code = 200;
          break;
        } else if (key[0] == '2' && key[1] == '0') {
          status_code = atoi(key);
          break;
        } else if (strcmp(key, "4XX") == 0) {
          status_code = 400;
        } else if (strcmp(key, "5XX") == 0) {
          status_code = 500;
        } else {
          int code = atoi(key);
          if (code >= 200 && code < 600 && status_code == 200) {
            status_code = code;
          }
        }
      }
    }
  }

  json_t *route_responses = json_array();
  json_t *mock_response = json_object();

  json_object_set_new(mock_response, "uuid",
                      json_string("default-response"));
  json_object_set_new(mock_response, "label", json_string(""));
  json_object_set_new(mock_response, "statusCode",
                      json_integer(status_code));
  json_object_set_new(mock_response, "latency", json_integer(0));
  json_object_set_new(mock_response, "bodyType", json_string("INLINE"));
  json_object_set_new(mock_response, "filePath", json_string(""));
  json_object_set_new(mock_response, "databucketID", json_string(""));
  json_object_set_new(mock_response, "sendFileAsBody", json_false());
  json_object_set_new(mock_response, "body", json_string("{}"));
  json_object_set_new(mock_response, "headers", json_array());
  json_object_set_new(mock_response, "rules", json_array());
  json_object_set_new(mock_response, "rulesOperator", json_string("OR"));
  json_object_set_new(mock_response, "disableTemplating", json_false());
  json_object_set_new(mock_response, "fallbackTo404", json_false());
  json_object_set_new(mock_response, "default", json_true());
  json_object_set_new(mock_response, "crudKey", json_string("id"));

  json_array_append(route_responses, mock_response);
  json_decref(mock_response);

  json_object_set_new(route, "responses", route_responses);

  return route;
}

openapi_result_t openapi_convert_to_environment(
    json_t *openapi_json, environment_t **env) {
  if (!openapi_json || !env) {
    return OPENAPI_CONVERSION_ERROR;
  }

  openapi_version_t version = openapi_detect_version(openapi_json);
  if (version == OPENAPI_UNKNOWN) {
    return OPENAPI_INVALID_VERSION;
  }

  json_t *info = json_object_get(openapi_json, "info");
  if (!info) {
    return OPENAPI_PARSE_ERROR;
  }

  json_t *title = json_object_get(info, "title");
  if (!json_is_string(title)) {
    return OPENAPI_PARSE_ERROR;
  }

  *env = (environment_t *)calloc(1, sizeof(environment_t));
  if (!*env) {
    return OPENAPI_CONVERSION_ERROR;
  }

  (*env)->name = strdup(json_string_value(title));
  (*env)->port = 3000;
  (*env)->proxy_mode = false;
  (*env)->last_migration = 32;

  json_t *paths = json_object_get(openapi_json, "paths");
  json_t *routes = json_array();

  if (json_is_object(paths)) {
    const char *path_key = NULL;
    json_t *path_obj = NULL;

    json_object_foreach(paths, path_key, path_obj) {
      if (json_is_object(path_obj)) {
        const char *method_key = NULL;
        json_t *operation = NULL;

        json_object_foreach(path_obj, method_key, operation) {
          if (json_is_object(operation) && method_key) {
            char *lower_method = strdup(method_key);
            if (lower_method) {
              for (char *p = lower_method; *p; p++) {
                *p = tolower(*p);
              }

              json_t *route = create_mockoon_route_from_openapi_path(
                  path_key, lower_method, operation);
              json_array_append(routes, route);
              json_decref(route);
              free(lower_method);
            }
          }
        }
      }
    }
  }

  (*env)->routes = routes;

  return OPENAPI_OK;
}

openapi_result_t openapi_convert_from_environment(
    environment_t *env, const char *format, char **result) {
  if (!env || !format || !result) {
    return OPENAPI_CONVERSION_ERROR;
  }

  json_t *openapi = json_object();

  json_object_set_new(openapi, "openapi", json_string("3.0.0"));

  json_t *info = json_object();
  json_object_set_new(info, "title",
                      json_string(env->name ? env->name : "Mockoon API"));
  json_object_set_new(info, "version", json_string("1.0.0"));
  json_object_set_new(openapi, "info", info);

  json_t *servers = json_array();
  json_t *server = json_object();

  char server_url[256];
  snprintf(server_url, sizeof(server_url), "http://localhost:%d",
           env->port);
  json_object_set_new(server, "url", json_string(server_url));
  json_array_append(servers, server);
  json_decref(server);

  json_object_set_new(openapi, "servers", servers);

  json_t *paths = json_object();

  if (env->routes && json_is_array(env->routes)) {
    size_t index = 0;
    json_t *route = NULL;

    json_array_foreach(env->routes, index, route) {
      if (json_is_object(route)) {
        json_t *path_obj = json_object_get(route, "path");
        json_t *method_obj = json_object_get(route, "method");

        if (json_is_string(path_obj) && json_is_string(method_obj)) {
          const char *path = json_string_value(path_obj);
          const char *method = json_string_value(method_obj);

          json_t *path_item = json_object_get(paths, path);
          if (!path_item) {
            path_item = json_object();
            json_object_set_new(paths, path, path_item);
          }

          json_t *operation = json_object();
          json_object_set_new(operation, "summary", json_string(""));
          json_object_set_new(operation, "tags", json_array());

          json_t *responses = json_object();
          json_t *response_200 = json_object();
          json_object_set_new(response_200, "description",
                              json_string("Successful response"));
          json_object_set_new(responses, "200", response_200);
          json_object_set_new(operation, "responses", responses);

          json_object_set(path_item, method, operation);
          json_decref(operation);
        }
      }
    }
  }

  json_object_set_new(openapi, "paths", paths);

  bool prettify = strcmp(format, "yaml") != 0;
  json_result_t json_result = json_to_string(openapi, prettify, result);

  json_decref(openapi);

  if (json_result != JSON_OK) {
    return OPENAPI_CONVERSION_ERROR;
  }

  return OPENAPI_OK;
}

const char* openapi_get_error_message(openapi_result_t result) {
  static const char *messages[] = {
    "OK",
    "Invalid OpenAPI version",
    "OpenAPI parse error",
    "OpenAPI conversion error",
    "Unsupported OpenAPI feature"
  };

  if (result < 0 || result > OPENAPI_UNSUPPORTED_FEATURE) {
    return "Unknown error";
  }

  return messages[result];
}
