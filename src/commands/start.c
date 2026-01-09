#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "core/json.h"
#include "core/schema.h"
#include "core/watcher.h"
#include "utils/colors.h"
#include "utils/args.h"
#include "commands/start.h"

static volatile int running = 1;

static void signal_handler(int sig) {
  (void)sig;
  running = 0;
}

int command_start(parsed_args_t *args) {
  if (!args || args->data_files_count == 0) {
    color_print_error("--data flag is required\n");
    return 1;
  }

  signal(SIGINT, signal_handler);

  for (int i = 0; i < args->data_files_count; i++) {
    const char *filepath = args->data_files[i];
    json_t *env_json = NULL;
    json_result_t json_result = json_parse_file(filepath, &env_json);

    if (json_result != JSON_OK) {
      color_print_error("Failed to load environment: %s\n",
                        json_get_error_message(json_result));
      return 1;
    }

    schema_validation_result_t *validation_result =
        schema_validate_environment(env_json);

    if (!validation_result->valid) {
      color_print_error("Invalid environment: %s\n", filepath);
      for (int j = 0; j < validation_result->error_count; j++) {
        color_print_error("  %s\n", validation_result->errors[j]);
      }
      schema_validation_result_free(validation_result);
      json_decref(env_json);
      return 1;
    }

    schema_validation_result_free(validation_result);
    json_decref(env_json);
  }

  color_print_success(
      "Mock API is running. Press Ctrl+C to stop.\n");

  watcher_t *watcher = NULL;
  if (args->watch && args->data_files_count > 0) {
    watcher = watcher_create(args->data_files[0],
                             args->polling_interval);
    if (watcher) {
      color_print_info("Watching for changes on %s...\n",
                       args->data_files[0]);
    }
  }

  while (running) {
    sleep(1);
  }

  if (watcher) {
    watcher_destroy(watcher);
  }

  color_print_info("Shutting down...\n");
  return 0;
}
