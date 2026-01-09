#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "core/json.h"
#include "core/file.h"
#include "utils/colors.h"

static const char* json_error_messages[] = {
  "OK",
  "JSON parse error",
  "Invalid JSON format",
  "Missing required field"
};

json_result_t json_parse_string(const char *json_str, json_t **result) {
  if (!json_str || !result) {
    return JSON_PARSE_ERROR;
  }

  json_error_t error;
  *result = json_loads(json_str, 0, &error);

  if (!*result) {
    return JSON_PARSE_ERROR;
  }

  return JSON_OK;
}

json_result_t json_parse_file(const char *filepath, json_t **result) {
  if (!filepath || !result) {
    return JSON_PARSE_ERROR;
  }

  char *content = NULL;
  file_result_t file_result = file_read_to_string(filepath, &content);

  if (file_result != FILE_OK) {
    return JSON_PARSE_ERROR;
  }

  json_result_t json_result = json_parse_string(content, result);
  free(content);

  return json_result;
}

json_result_t json_to_string(json_t *obj, bool prettify, char **result) {
  if (!obj || !result) {
    return JSON_PARSE_ERROR;
  }

  if (prettify) {
    *result = json_dumps(obj, JSON_INDENT(2) | JSON_SORT_KEYS);
  } else {
    *result = json_dumps(obj, JSON_COMPACT | JSON_SORT_KEYS);
  }

  if (!*result) {
    return JSON_PARSE_ERROR;
  }

  return JSON_OK;
}

json_result_t json_load_environment(json_t *obj, environment_t **env) {
  if (!obj || !env) {
    return JSON_INVALID_FORMAT;
  }

  *env = (environment_t *)calloc(1, sizeof(environment_t));
  if (!*env) {
    return JSON_INVALID_FORMAT;
  }

  json_t *name_obj = json_object_get(obj, "name");
  if (json_is_string(name_obj)) {
    (*env)->name = strdup(json_string_value(name_obj));
  } else {
    return JSON_MISSING_FIELD;
  }

  json_t *port_obj = json_object_get(obj, "port");
  if (json_is_integer(port_obj)) {
    (*env)->port = json_integer_value(port_obj);
  } else {
    return JSON_MISSING_FIELD;
  }

  json_t *hostname_obj = json_object_get(obj, "hostname");
  if (json_is_string(hostname_obj)) {
    char *hostname = strdup(json_string_value(hostname_obj));
    (*env)->hostname = &hostname;
    (*env)->hostname_len = 1;
  }

  json_t *proxy_obj = json_object_get(obj, "proxyMode");
  if (json_is_boolean(proxy_obj)) {
    (*env)->proxy_mode = json_boolean_value(proxy_obj);
  }

  json_t *migration_obj = json_object_get(obj, "lastMigration");
  if (json_is_integer(migration_obj)) {
    (*env)->last_migration = json_integer_value(migration_obj);
  }

  (*env)->routes = json_object_get(obj, "routes");
  (*env)->cors_rules = json_object_get(obj, "corsRules");
  (*env)->data_buckets = json_object_get(obj, "dataBuckets");
  (*env)->callbacks = json_object_get(obj, "callbacks");
  (*env)->variables = json_object_get(obj, "variables");

  return JSON_OK;
}

json_result_t json_validate_environment(json_t *obj) {
  if (!obj || !json_is_object(obj)) {
    return JSON_INVALID_FORMAT;
  }

  json_t *name = json_object_get(obj, "name");
  if (!json_is_string(name)) {
    return JSON_MISSING_FIELD;
  }

  json_t *port = json_object_get(obj, "port");
  if (!json_is_integer(port)) {
    return JSON_MISSING_FIELD;
  }

  return JSON_OK;
}

void json_free_environment(environment_t *env) {
  if (!env) {
    return;
  }

  if (env->name) {
    free(env->name);
  }

  if (env->hostname) {
    for (int i = 0; i < env->hostname_len; i++) {
      free(env->hostname[i]);
    }
    free(env->hostname);
  }

  free(env);
}

char* json_get_error_message(json_result_t result) {
  if (result < 0 || result > JSON_MISSING_FIELD) {
    return (char *)"Unknown error";
  }
  return (char *)json_error_messages[result];
}

json_t* json_environment_to_json(environment_t *env) {
  if (!env) {
    return NULL;
  }

  json_t *obj = json_object();

  json_object_set_new(obj, "uuid", json_string("imported-api"));
  json_object_set_new(obj, "lastMigration", json_integer(env->last_migration));

  if (env->name) {
    json_object_set_new(obj, "name", json_string(env->name));
  }

  json_object_set_new(obj, "endpointPrefix", json_string(""));
  json_object_set_new(obj, "latency", json_integer(0));
  json_object_set_new(obj, "port", json_integer(env->port));

  if (env->hostname && env->hostname_len > 0) {
    json_object_set_new(obj, "hostname", json_string(env->hostname[0]));
  } else {
    json_object_set_new(obj, "hostname", json_string("0.0.0.0"));
  }

  json_object_set_new(obj, "folders", json_array());

  if (env->routes) {
    json_object_set(obj, "routes", env->routes);
  } else {
    json_object_set_new(obj, "routes", json_array());
  }

  json_t *root_children = json_array();
  if (env->routes && json_is_array(env->routes)) {
    size_t index;
    json_t *route;
    json_array_foreach(env->routes, index, route) {
      json_t *child = json_object();
      json_object_set_new(child, "type", json_string("route"));
      json_t *uuid = json_object_get(route, "uuid");
      if (uuid) {
        json_object_set(child, "uuid", uuid);
      }
      json_array_append_new(root_children, child);
    }
  }
  json_object_set_new(obj, "rootChildren", root_children);

  json_object_set_new(obj, "proxyMode", json_boolean(env->proxy_mode));
  json_object_set_new(obj, "proxyHost", json_string(""));
  json_object_set_new(obj, "proxyRemovePrefix", json_false());

  json_t *tls_options = json_object();
  json_object_set_new(tls_options, "enabled", json_false());
  json_object_set_new(tls_options, "type", json_string("CERT"));
  json_object_set_new(tls_options, "pfxPath", json_string(""));
  json_object_set_new(tls_options, "certPath", json_string(""));
  json_object_set_new(tls_options, "keyPath", json_string(""));
  json_object_set_new(tls_options, "caPath", json_string(""));
  json_object_set_new(tls_options, "passphrase", json_string(""));
  json_object_set_new(obj, "tlsOptions", tls_options);

  json_object_set_new(obj, "cors", json_true());

  json_t *headers = json_array();
  json_t *header = json_object();
  json_object_set_new(header, "key", json_string("Content-Type"));
  json_object_set_new(header, "value", json_string("application/json"));
  json_array_append_new(headers, header);
  json_object_set_new(obj, "headers", headers);

  json_t *empty_headers = json_array();
  json_t *empty_header = json_object();
  json_object_set_new(empty_header, "key", json_string(""));
  json_object_set_new(empty_header, "value", json_string(""));
  json_array_append_new(empty_headers, empty_header);
  json_object_set_new(obj, "proxyReqHeaders", json_copy(empty_headers));
  json_object_set_new(obj, "proxyResHeaders", empty_headers);

  if (env->data_buckets) {
    json_object_set(obj, "data", env->data_buckets);
  } else {
    json_object_set_new(obj, "data", json_array());
  }

  if (env->callbacks) {
    json_object_set(obj, "callbacks", env->callbacks);
  } else {
    json_object_set_new(obj, "callbacks", json_array());
  }

  return obj;
}
