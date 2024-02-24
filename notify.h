#ifndef NOTIFY_H
#define NOTIFY_H

#include <raylib.h>
#include "frame.h"

typedef struct NotifyArgs {
  int state;
  bool b;
  Vector2 v2;
  Texture tex;
  frame_h *frame_pointer;
  int frame_array_len;
} NotifyArgs;

typedef enum NotifyMessage {
  TEXTURE_FRAME_CREATED = 1,
  STATE_ENTERED,
} NotifyMessage;

typedef void (*NotifyCallback)(NotifyArgs args);

void init_notification_system();
void notify(NotifyMessage m, NotifyArgs args);
void register_listener(NotifyMessage m, NotifyCallback callback);

#endif
