#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils/args.h"

static parsed_args_t* args_create(void) {
  parsed_args_t *args = (parsed_args_t *)calloc(1, sizeof(parsed_args_t));
  return args;
}

parsed_args_t* parse_arguments(int argc, char *argv[], const char *command) {
  parsed_args_t *args = args_create();
  if (!args) {
    return NULL;
  }

  args->faker_locale = "en";
  args->faker_seed = -1;
  args->env_vars_prefix = "MOCKOON_";
  args->max_transaction_logs = 100;
  args->polling_interval = 2000;
  args->format = "json";

  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];

    if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
      args->help = true;
    } else if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0) {
      args->version = true;
    } else if (strcmp(arg, "--data") == 0 || strcmp(arg, "-d") == 0) {
      if (i + 1 < argc) {
        args->data_files = (char **)realloc(args->data_files,
                                            sizeof(char *) *
                                            (args->data_files_count + 1));
        args->data_files[args->data_files_count++] = argv[++i];
      }
    } else if (strcmp(arg, "--port") == 0 || strcmp(arg, "-p") == 0) {
      if (i + 1 < argc) {
        args->ports = (int *)realloc(args->ports,
                                     sizeof(int) * (args->ports_count + 1));
        args->ports[args->ports_count++] = atoi(argv[++i]);
      }
    } else if (strcmp(arg, "--hostname") == 0 || strcmp(arg, "-l") == 0) {
      if (i + 1 < argc) {
        args->hostnames = (char **)realloc(args->hostnames,
                                           sizeof(char *) *
                                           (args->hostnames_count + 1));
        args->hostnames[args->hostnames_count++] = argv[++i];
      }
    } else if (strcmp(arg, "--log-transaction") == 0 ||
               strcmp(arg, "-t") == 0) {
      args->log_transaction = true;
    } else if (strcmp(arg, "--watch") == 0 || strcmp(arg, "-w") == 0) {
      args->watch = true;
    } else if (strcmp(arg, "--polling-interval") == 0) {
      if (i + 1 < argc) {
        args->polling_interval = atoi(argv[++i]);
      }
    } else if (strcmp(arg, "--repair") == 0 || strcmp(arg, "-r") == 0) {
      args->repair = true;
    } else if (strcmp(arg, "--disable-log-to-file") == 0 ||
               strcmp(arg, "-X") == 0) {
      args->disable_log_to_file = true;
    } else if (strcmp(arg, "--disable-routes") == 0 || strcmp(arg, "-e") == 0) {
      if (i + 1 < argc) {
        args->disable_routes = (char **)realloc(args->disable_routes,
                                                sizeof(char *) *
                                                (args->disable_routes_count +
                                                 1));
        args->disable_routes[args->disable_routes_count++] = argv[++i];
      }
    } else if (strcmp(arg, "--faker-locale") == 0 || strcmp(arg, "-c") == 0) {
      if (i + 1 < argc) {
        args->faker_locale = argv[++i];
      }
    } else if (strcmp(arg, "--faker-seed") == 0 || strcmp(arg, "-s") == 0) {
      if (i + 1 < argc) {
        args->faker_seed = atoi(argv[++i]);
      }
    } else if (strcmp(arg, "--env-vars-prefix") == 0 || strcmp(arg, "-x") == 0) {
      if (i + 1 < argc) {
        args->env_vars_prefix = argv[++i];
      }
    } else if (strcmp(arg, "--disable-admin-api") == 0) {
      args->disable_admin_api = true;
    } else if (strcmp(arg, "--disable-tls") == 0) {
      args->disable_tls = true;
    } else if (strcmp(arg, "--max-transaction-logs") == 0) {
      if (i + 1 < argc) {
        args->max_transaction_logs = atoi(argv[++i]);
      }
    } else if (strcmp(arg, "--enable-random-latency") == 0) {
      args->enable_random_latency = true;
    } else if (strcmp(arg, "--proxy") == 0) {
      if (i + 1 < argc) {
        args->proxy_mode = argv[++i];
      }
    } else if (strcmp(arg, "--token") == 0 || strcmp(arg, "-k") == 0) {
      if (i + 1 < argc) {
        args->token = argv[++i];
      }
    } else if (strcmp(arg, "--input") == 0 || strcmp(arg, "-i") == 0) {
      if (i + 1 < argc) {
        args->input_file = argv[++i];
      }
    } else if (strcmp(arg, "--output") == 0 || strcmp(arg, "-o") == 0) {
      if (i + 1 < argc) {
        args->output_file = argv[++i];
      }
    } else if (strcmp(arg, "--prettify") == 0 || strcmp(arg, "-p") == 0) {
      args->prettify = true;
    } else if (strcmp(arg, "--format") == 0 || strcmp(arg, "-f") == 0) {
      if (i + 1 < argc) {
        args->format = argv[++i];
      }
    }
  }

  return args;
}

void args_free(parsed_args_t *args) {
  if (!args) {
    return;
  }
  if (args->data_files) {
    free(args->data_files);
  }
  if (args->ports) {
    free(args->ports);
  }
  if (args->hostnames) {
    free(args->hostnames);
  }
  if (args->disable_routes) {
    free(args->disable_routes);
  }
  free(args);
}

const char* get_arg_value(parsed_args_t *args, const char *long_opt) {
  if (!args || !long_opt) {
    return NULL;
  }

  if (strcmp(long_opt, "token") == 0) {
    return args->token;
  } else if (strcmp(long_opt, "faker-locale") == 0) {
    return args->faker_locale;
  } else if (strcmp(long_opt, "env-vars-prefix") == 0) {
    return args->env_vars_prefix;
  }

  return NULL;
}

bool has_arg(parsed_args_t *args, const char *long_opt) {
  if (!args || !long_opt) {
    return false;
  }

  if (strcmp(long_opt, "help") == 0) {
    return args->help;
  } else if (strcmp(long_opt, "version") == 0) {
    return args->version;
  } else if (strcmp(long_opt, "log-transaction") == 0) {
    return args->log_transaction;
  } else if (strcmp(long_opt, "watch") == 0) {
    return args->watch;
  } else if (strcmp(long_opt, "repair") == 0) {
    return args->repair;
  } else if (strcmp(long_opt, "disable-log-to-file") == 0) {
    return args->disable_log_to_file;
  } else if (strcmp(long_opt, "disable-admin-api") == 0) {
    return args->disable_admin_api;
  } else if (strcmp(long_opt, "disable-tls") == 0) {
    return args->disable_tls;
  } else if (strcmp(long_opt, "enable-random-latency") == 0) {
    return args->enable_random_latency;
  } else if (strcmp(long_opt, "prettify") == 0) {
    return args->prettify;
  }

  return false;
}
