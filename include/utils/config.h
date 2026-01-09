#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
  char *logs_path;
  char *cloud_scheme;
  char *cloud_api_url;
  char *version;
  bool is_production;
} config_t;

config_t* config_get(void);
void config_init(void);
void config_destroy(void);
char* config_get_logs_path(void);
char* config_get_cloud_api_url(void);
bool config_is_production(void);

#endif /* CONFIG_H */
