#ifndef COLORS_H
#define COLORS_H

#define COLOR_RESET "\x1b[39m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_GRAY "\x1b[90m"

void color_print_error(const char *format, ...);
void color_print_success(const char *format, ...);
void color_print_info(const char *format, ...);
void color_print_warning(const char *format, ...);
void color_print_question(const char *message);
char* color_get_error_marker(void);
char* color_get_success_marker(void);

#endif /* COLORS_H */
