#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/file.h"
#include "utils/colors.h"
#include "utils/args.h"
#include "commands/dockerize.h"

int command_dockerize(parsed_args_t *args) {
  if (!args) {
    color_print_error("Invalid arguments\n");
    return 1;
  }

  if (args->data_files_count == 0) {
    color_print_error("--data flag is required\n");
    return 1;
  }

  if (!args->output_file) {
    color_print_error("--output flag is required\n");
    return 1;
  }

  const char *data_file = args->data_files[0];
  int port = args->ports_count > 0 ? args->ports[0] : 3000;

  char dockerfile_content[2048];
  snprintf(dockerfile_content, sizeof(dockerfile_content),
           "FROM node:18-alpine\n"
           "WORKDIR /app\n"
           "RUN npm install -g @mockoon/cli\n"
           "COPY . .\n"
           "EXPOSE %d\n"
           "CMD [\"mockoon-cli\", \"start\", \"-d\", \"/app/%s\"]\n",
           port, data_file);

  file_result_t file_result =
      file_write_string(args->output_file, dockerfile_content);

  if (file_result != FILE_OK) {
    color_print_error("Failed to write Dockerfile\n");
    return 1;
  }

  color_print_success("Generated Dockerfile at %s\n", args->output_file);
  color_print_info("Next steps:\n");
  color_print_info("  1. Copy your data file to the same directory as Dockerfile\n");
  color_print_info("  2. Run: docker build -t mockoon-api .\n");
  color_print_info("  3. Run: docker run -p %d:%d mockoon-api\n", port, port);

  return 0;
}
