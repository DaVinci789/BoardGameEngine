#ifndef NOTIFY_H
#define NOTIFY_H

#include <raylib.h>

typedef struct NotifyArgs {
  int state;
  bool b;
  Vector2 v2;
  Texture tex;
} NotifyArgs;

typedef enum NotifyMessage {
  TEXTURE_FRAME_CREATED = 1,
  STATE_ENTERED,
  QUERY_CARD,
  QUERY_CARD_FINISHED,
} NotifyMessage;

typedef void (*NotifyCallback)(NotifyArgs args);

void init_notification_system();
void notify(NotifyMessage m, NotifyArgs args);
void register_listener(NotifyMessage m, NotifyCallback callback);

#endif
