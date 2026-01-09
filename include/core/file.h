#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

typedef enum {
  FILE_OK,
  FILE_NOT_FOUND,
  FILE_READ_ERROR,
  FILE_WRITE_ERROR,
  FILE_PERMISSION_ERROR
} file_result_t;

file_result_t file_read_to_string(const char *filepath, char **content);
file_result_t file_write_string(const char *filepath, const char *content);
file_result_t file_exists(const char *filepath, bool *exists);
file_result_t file_is_url(const char *path, bool *is_url);
char* file_get_directory(const char *filepath);
char* file_get_filename(const char *filepath);
void file_free_string(char *str);

#endif /* FILE_H */
