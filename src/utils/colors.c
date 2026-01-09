#include <stdio.h>
#include <stdarg.h>
#include "utils/colors.h"

void color_print_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s»%s   ", COLOR_RED, COLOR_RESET);
  vprintf(format, args);
  va_end(args);
}

void color_print_success(const char *format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s✓%s ", COLOR_GREEN, COLOR_RESET);
  vprintf(format, args);
  va_end(args);
}

void color_print_info(const char *format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s»%s ", COLOR_BLUE, COLOR_RESET);
  vprintf(format, args);
  va_end(args);
}

void color_print_warning(const char *format, ...) {
  va_list args;
  va_start(args, format);
  printf("%s⚠%s ", COLOR_BLUE, COLOR_RESET);
  vprintf(format, args);
  va_end(args);
}

void color_print_question(const char *message) {
  printf("%s?%s %s %s(y/n)%s ", COLOR_BLUE, COLOR_RESET, message,
         COLOR_GRAY, COLOR_RESET);
  fflush(stdout);
}

char* color_get_error_marker(void) {
  return COLOR_RED "»" COLOR_RESET;
}

char* color_get_success_marker(void) {
  return COLOR_GREEN "✓" COLOR_RESET;
}
