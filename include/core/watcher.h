#ifndef WATCHER_H
#define WATCHER_H

#include <stdbool.h>

typedef void (*file_change_callback_t)(const char *filepath, void *user_data);

typedef struct watcher_s watcher_t;

watcher_t* watcher_create(const char *filepath, int polling_interval_ms);
bool watcher_start(watcher_t *watcher, file_change_callback_t callback,
                   void *user_data);
void watcher_stop(watcher_t *watcher);
void watcher_destroy(watcher_t *watcher);

#endif /* WATCHER_H */
