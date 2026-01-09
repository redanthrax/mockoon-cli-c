#include <stdio.h>
#include <stdlib.h>
#include "core/json.h"
#include "core/schema.h"
#include "utils/colors.h"
#include "utils/args.h"
#include "commands/validate.h"

int command_validate(parsed_args_t *args) {
  if (!args || args->data_files_count == 0) {
    color_print_error("--data flag is required\n");
    return 1;
  }

  bool has_errors = false;

  for (int i = 0; i < args->data_files_count; i++) {
    const char *filepath = args->data_files[i];
    json_t *env_json = NULL;
    json_result_t json_result = json_parse_file(filepath, &env_json);

    if (json_result != JSON_OK) {
      color_print_error("Invalid environment: %s\n", filepath);
      color_print_error("  %s\n", json_get_error_message(json_result));
      has_errors = true;
      continue;
    }

    schema_validation_result_t *validation_result =
        schema_validate_environment(env_json);

    if (!validation_result->valid) {
      color_print_error("Invalid environment: %s\n", filepath);
      for (int j = 0; j < validation_result->error_count; j++) {
        color_print_error("  %s\n", validation_result->errors[j]);
      }
      has_errors = true;
    } else {
      color_print_success("Valid environment: %s\n", filepath);
    }

    schema_validation_result_free(validation_result);
    json_decref(env_json);
  }

  if (has_errors) {
    color_print_error("Environments validation failed\n");
    return 1;
  }

  color_print_success("All environments are valid\n");
  return 0;
}
