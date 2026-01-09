#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "core/watcher.h"

struct watcher_s {
  char *filepath;
  int polling_interval_ms;
  file_change_callback_t callback;
  void *user_data;
  bool running;
  pthread_t thread;
  time_t last_mtime;
};

static void* watcher_thread_func(void *arg) {
  watcher_t *watcher = (watcher_t *)arg;

  while (watcher->running) {
    struct stat file_stat;
    if (stat(watcher->filepath, &file_stat) == 0) {
      if (watcher->last_mtime > 0 && file_stat.st_mtime > watcher->last_mtime) {
        if (watcher->callback) {
          watcher->callback(watcher->filepath, watcher->user_data);
        }
      }
      watcher->last_mtime = file_stat.st_mtime;
    }

    usleep(watcher->polling_interval_ms * 1000);
  }

  return NULL;
}

watcher_t* watcher_create(const char *filepath, int polling_interval_ms) {
  if (!filepath || polling_interval_ms < 100) {
    return NULL;
  }

  watcher_t *watcher = (watcher_t *)calloc(1, sizeof(watcher_t));
  if (!watcher) {
    return NULL;
  }

  watcher->filepath = strdup(filepath);
  if (!watcher->filepath) {
    free(watcher);
    return NULL;
  }

  watcher->polling_interval_ms = polling_interval_ms;
  watcher->running = false;
  watcher->callback = NULL;
  watcher->user_data = NULL;
  watcher->last_mtime = 0;

  return watcher;
}

bool watcher_start(watcher_t *watcher, file_change_callback_t callback,
                   void *user_data) {
  if (!watcher || !callback) {
    return false;
  }

  struct stat file_stat;
  if (stat(watcher->filepath, &file_stat) != 0) {
    return false;
  }

  watcher->callback = callback;
  watcher->user_data = user_data;
  watcher->last_mtime = file_stat.st_mtime;
  watcher->running = true;

  if (pthread_create(&watcher->thread, NULL, watcher_thread_func,
                     watcher) != 0) {
    watcher->running = false;
    return false;
  }

  return true;
}

void watcher_stop(watcher_t *watcher) {
  if (!watcher) {
    return;
  }

  watcher->running = false;

  if (watcher->thread) {
    pthread_join(watcher->thread, NULL);
  }
}

void watcher_destroy(watcher_t *watcher) {
  if (!watcher) {
    return;
  }

  watcher_stop(watcher);

  if (watcher->filepath) {
    free(watcher->filepath);
  }

  free(watcher);
}
