#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include "core/file.h"

file_result_t file_read_to_string(const char *filepath, char **content) {
  if (!filepath || !content) {
    return FILE_READ_ERROR;
  }

  FILE *fp = fopen(filepath, "r");
  if (!fp) {
    return FILE_NOT_FOUND;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (file_size < 0) {
    fclose(fp);
    return FILE_READ_ERROR;
  }

  *content = (char *)malloc(file_size + 1);
  if (!*content) {
    fclose(fp);
    return FILE_READ_ERROR;
  }

  size_t bytes_read = fread(*content, 1, file_size, fp);
  fclose(fp);

  if (bytes_read != (size_t)file_size) {
    free(*content);
    *content = NULL;
    return FILE_READ_ERROR;
  }

  (*content)[file_size] = '\0';
  return FILE_OK;
}

file_result_t file_write_string(const char *filepath, const char *content) {
  if (!filepath || !content) {
    return FILE_WRITE_ERROR;
  }

  FILE *fp = fopen(filepath, "w");
  if (!fp) {
    return FILE_PERMISSION_ERROR;
  }

  size_t content_len = strlen(content);
  size_t written = fwrite(content, 1, content_len, fp);
  fclose(fp);

  if (written != content_len) {
    return FILE_WRITE_ERROR;
  }

  return FILE_OK;
}

file_result_t file_exists(const char *filepath, bool *exists) {
  if (!filepath || !exists) {
    return FILE_READ_ERROR;
  }

  struct stat buffer;
  *exists = (stat(filepath, &buffer) == 0);
  return FILE_OK;
}

file_result_t file_is_url(const char *path, bool *is_url) {
  if (!path || !is_url) {
    return FILE_READ_ERROR;
  }

  *is_url = (strncmp(path, "http://", 7) == 0 ||
             strncmp(path, "https://", 8) == 0 ||
             strncmp(path, "cloud://", 8) == 0);
  return FILE_OK;
}

char* file_get_directory(const char *filepath) {
  if (!filepath) {
    return NULL;
  }

  bool is_url;
  file_is_url(filepath, &is_url);
  if (is_url) {
    return NULL;
  }

  char *path_copy = strdup(filepath);
  if (!path_copy) {
    return NULL;
  }

  char *dir = dirname(path_copy);
  char *result = strdup(dir);
  free(path_copy);

  return result;
}

char* file_get_filename(const char *filepath) {
  if (!filepath) {
    return NULL;
  }

  char *path_copy = strdup(filepath);
  if (!path_copy) {
    return NULL;
  }

  char *filename = strdup(basename(path_copy));
  free(path_copy);

  return filename;
}

void file_free_string(char *str) {
  if (str) {
    free(str);
  }
}
