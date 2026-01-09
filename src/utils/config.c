#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include "utils/config.h"
#include "mockoon.h"

static config_t *g_config = NULL;

static char* get_home_directory(void) {
  struct passwd *pw = getpwuid(getuid());
  if (pw) {
    return pw->pw_dir;
  }
  return getenv("HOME");
}

void config_init(void) {
  g_config = (config_t *)malloc(sizeof(config_t));
  if (!g_config) {
    return;
  }

  g_config->version = MOCKOON_VERSION;
  g_config->cloud_scheme = "cloud://";
  g_config->is_production = 0;

  const char *env = getenv("NODE_ENV");
  if (env && strcmp(env, "production") == 0) {
    g_config->is_production = 1;
    g_config->cloud_api_url = "https://api.mockoon.com/environments";
  } else {
    g_config->cloud_api_url = "http://localhost:5003/environments";
  }

  char *home = get_home_directory();
  if (home) {
    size_t path_len = strlen(home) + strlen("/.mockoon-cli/logs/") + 1;
    g_config->logs_path = (char *)malloc(path_len);
    if (g_config->logs_path) {
      snprintf(g_config->logs_path, path_len, "%s/.mockoon-cli/logs/",
               home);
    }
  }
}

void config_destroy(void) {
  if (g_config) {
    if (g_config->logs_path) {
      free(g_config->logs_path);
    }
    free(g_config);
    g_config = NULL;
  }
}

config_t* config_get(void) {
  return g_config;
}

char* config_get_logs_path(void) {
  return g_config ? g_config->logs_path : NULL;
}

char* config_get_cloud_api_url(void) {
  return g_config ? g_config->cloud_api_url : NULL;
}

bool config_is_production(void) {
  return g_config ? g_config->is_production : false;
}
