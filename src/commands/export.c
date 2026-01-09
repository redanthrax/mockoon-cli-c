#include <stdio.h>
#include <stdlib.h>
#include "core/json.h"
#include "core/openapi.h"
#include "core/file.h"
#include "utils/colors.h"
#include "utils/args.h"
#include "commands/export.h"

int command_export(parsed_args_t *args) {
  if (!args) {
    color_print_error("Invalid arguments\n");
    return 1;
  }

  if (!args->input_file) {
    color_print_error("--input flag is required\n");
    return 1;
  }

  if (!args->output_file) {
    color_print_error("--output flag is required\n");
    return 1;
  }

  json_t *env_json = NULL;
  json_result_t json_result = json_parse_file(args->input_file, &env_json);

  if (json_result != JSON_OK) {
    color_print_error("Failed to parse environment file: %s\n",
                      json_get_error_message(json_result));
    return 1;
  }

  environment_t *env = NULL;
  json_result = json_load_environment(env_json, &env);
  json_decref(env_json);

  if (json_result != JSON_OK) {
    color_print_error("Failed to load environment: %s\n",
                      json_get_error_message(json_result));
    return 1;
  }

  const char *format = args->format ? args->format : "json";
  char *openapi_str = NULL;
  openapi_result_t openapi_result =
      openapi_convert_from_environment(env, format, &openapi_str);
  json_free_environment(env);

  if (openapi_result != OPENAPI_OK) {
    color_print_error("Failed to convert to OpenAPI: %s\n",
                      openapi_get_error_message(openapi_result));
    return 1;
  }

  file_result_t file_result =
      file_write_string(args->output_file, openapi_str);
  free(openapi_str);

  if (file_result != FILE_OK) {
    color_print_error("Failed to write output file\n");
    return 1;
  }

  color_print_success("Successfully exported to OpenAPI v3 at %s\n",
                      args->output_file);
  return 0;
}
