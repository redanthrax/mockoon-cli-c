#ifndef MOCKOON_H
#define MOCKOON_H

#include <stdbool.h>
#include <jansson.h>

#define MOCKOON_VERSION "1.0.0"
#define MOCKOON_NAME "mockoon-cli"

typedef enum {
  COMMAND_START,
  COMMAND_VALIDATE,
  COMMAND_IMPORT,
  COMMAND_EXPORT,
  COMMAND_DOCKERIZE,
  COMMAND_HELP,
  COMMAND_UNKNOWN
} command_type_t;

typedef struct {
  command_type_t command;
  int argc;
  char **argv;
} cli_context_t;

typedef struct {
  char *name;
  char *description;
  int port;
  int hostname_len;
  char **hostname;
  bool proxy_mode;
  int last_migration;
  json_t *routes;
  json_t *cors_rules;
  json_t *data_buckets;
  json_t *callbacks;
  json_t *variables;
} environment_t;

typedef enum {
  VALIDATION_OK,
  VALIDATION_SCHEMA_ERROR,
  VALIDATION_FILE_ERROR,
  VALIDATION_PARSE_ERROR
} validation_result_t;

typedef struct {
  validation_result_t result;
  char *message;
  json_t *details;
} validation_response_t;

#endif /* MOCKOON_H */
