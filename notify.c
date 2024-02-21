#include "notify.h"

#include <stdlib.h>
#include <stdio.h>

struct NotifyMessageCallbackPair
{
  NotifyMessage m;
  NotifyCallback c;
};

static struct NotifyMessageCallbackPair *pairs;
static int next_pair = 0;

void init_notification_system()
{
  pairs = calloc(1 << 16, sizeof(*pairs));
}

void notify(NotifyMessage m, NotifyArgs args)
{
  for (int i = 0; i < 1 << 16; i++) {
    struct NotifyMessageCallbackPair pair = pairs[i];
    if (pair.m == m) {
      pair.c(args);
    }
  }
}

void register_listener(NotifyMessage m, NotifyCallback callback)
{
  pairs[next_pair].m = m;
  pairs[next_pair].c = callback;
  next_pair += 1;
}
