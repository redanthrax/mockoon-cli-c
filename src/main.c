#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mockoon.h"
#include "utils/config.h"
#include "utils/colors.h"
#include "utils/args.h"
#include "commands/validate.h"
#include "commands/import.h"
#include "commands/export.h"
#include "commands/start.h"
#include "commands/dockerize.h"

void print_version(void) {
  printf("%s %s\n", MOCKOON_NAME, MOCKOON_VERSION);
}

void print_help(void) {
  printf("Mockoon CLI - Mock API server for local development\n\n");
  printf("Usage: %s <command> [options]\n\n", MOCKOON_NAME);
  printf("Commands:\n");
  printf("  start     - Start one or more mock API\n");
  printf("  validate  - Validate a Mockoon environment JSON file\n");
  printf("  import    - Import an OpenAPI/Swagger specification\n");
  printf("  export    - Export a mock API to OpenAPI v3\n");
  printf("  dockerize - Generate a Dockerfile for your mock API\n");
  printf("  help      - Display this help message\n");
  printf("  version   - Display version information\n\n");
  printf("For command-specific help: %s <command> --help\n", MOCKOON_NAME);
}

command_type_t parse_command(const char *cmd) {
  if (cmd == NULL) {
    return COMMAND_UNKNOWN;
  }

  if (strcmp(cmd, "start") == 0) {
    return COMMAND_START;
  } else if (strcmp(cmd, "validate") == 0) {
    return COMMAND_VALIDATE;
  } else if (strcmp(cmd, "import") == 0) {
    return COMMAND_IMPORT;
  } else if (strcmp(cmd, "export") == 0) {
    return COMMAND_EXPORT;
  } else if (strcmp(cmd, "dockerize") == 0) {
    return COMMAND_DOCKERIZE;
  } else if (strcmp(cmd, "help") == 0) {
    return COMMAND_HELP;
  } else if (strcmp(cmd, "version") == 0 || strcmp(cmd, "--version") == 0 ||
             strcmp(cmd, "-v") == 0) {
    return COMMAND_HELP;
  }

  return COMMAND_UNKNOWN;
}

int main(int argc, char *argv[]) {
  config_init();

  if (argc < 2) {
    print_help();
    config_destroy();
    return 0;
  }

  command_type_t command = parse_command(argv[1]);

  if (command == COMMAND_UNKNOWN) {
    color_print_error("Unknown command: %s\n", argv[1]);
    print_help();
    config_destroy();
    return 1;
  }

  if (command == COMMAND_HELP) {
    print_version();
    print_help();
    config_destroy();
    return 0;
  }

  parsed_args_t *args = parse_arguments(argc - 1, argv + 1,
                                        argv[1]);
  int result = 0;

  switch (command) {
  case COMMAND_START:
    result = command_start(args);
    break;
  case COMMAND_VALIDATE:
    result = command_validate(args);
    break;
  case COMMAND_IMPORT:
    result = command_import(args);
    break;
  case COMMAND_EXPORT:
    result = command_export(args);
    break;
  case COMMAND_DOCKERIZE:
    result = command_dockerize(args);
    break;
  default:
    print_help();
    result = 1;
    break;
  }

  args_free(args);
  config_destroy();
  return result;
}
