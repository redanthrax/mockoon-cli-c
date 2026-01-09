#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

typedef struct {
  bool help;
  bool version;
  char **data_files;
  int data_files_count;
  int *ports;
  int ports_count;
  char **hostnames;
  int hostnames_count;
  bool log_transaction;
  bool watch;
  int polling_interval;
  bool repair;
  bool disable_log_to_file;
  char **disable_routes;
  int disable_routes_count;
  char *faker_locale;
  int faker_seed;
  char *env_vars_prefix;
  bool disable_admin_api;
  bool disable_tls;
  int max_transaction_logs;
  bool enable_random_latency;
  char *proxy_mode;
  char *token;
  char *input_file;
  char *output_file;
  bool prettify;
  char *format;
} parsed_args_t;

typedef struct {
  char *short_opt;
  char *long_opt;
  bool has_arg;
  char *description;
} option_def_t;

parsed_args_t* parse_arguments(int argc, char *argv[], const char *command);
void args_free(parsed_args_t *args);
const char* get_arg_value(parsed_args_t *args, const char *long_opt);
bool has_arg(parsed_args_t *args, const char *long_opt);

#endif /* ARGS_H */
