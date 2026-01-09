#include <stdio.h>
#include <stdlib.h>
#include "core/json.h"
#include "core/openapi.h"
#include "core/file.h"
#include "utils/colors.h"
#include "utils/args.h"
#include "commands/import.h"

int command_import(parsed_args_t *args) {
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

  json_t *openapi_json = NULL;
  json_result_t json_result = json_parse_file(args->input_file, &openapi_json);

  if (json_result != JSON_OK) {
    color_print_error("Failed to parse OpenAPI file: %s\n",
                      json_get_error_message(json_result));
    return 1;
  }

  environment_t *env = NULL;
  openapi_result_t openapi_result =
      openapi_convert_to_environment(openapi_json, &env);

  json_decref(openapi_json);

  if (openapi_result != OPENAPI_OK) {
    color_print_error("Failed to convert OpenAPI: %s\n",
                      openapi_get_error_message(openapi_result));
    return 1;
  }

  json_t *env_json = json_environment_to_json(env);
  json_free_environment(env);

  char *json_str = NULL;
  json_result = json_to_string(env_json, args->prettify, &json_str);
  json_decref(env_json);

  if (json_result != JSON_OK) {
    color_print_error("Failed to serialize environment: %s\n",
                      json_get_error_message(json_result));
    return 1;
  }

  file_result_t file_result =
      file_write_string(args->output_file, json_str);
  free(json_str);

  if (file_result != FILE_OK) {
    color_print_error("Failed to write output file\n");
    return 1;
  }

  color_print_success("Successfully imported OpenAPI spec to %s\n",
                      args->output_file);
  return 0;
}
