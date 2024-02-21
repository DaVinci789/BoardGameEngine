#ifndef NOTIFY_H
#define NOTIFY_H

#include <raylib.h>

typedef struct NotifyArgs {
  Vector2 v2;
  Texture tex;
} NotifyArgs;

typedef enum NotifyMessage {
  TEXTURE_FRAME_CREATED = 1,
} NotifyMessage;

typedef void (*NotifyCallback)(NotifyArgs args);

void init_notification_system();
void notify(NotifyMessage m, NotifyArgs args);
void register_listener(NotifyMessage m, NotifyCallback callback);

#endif
