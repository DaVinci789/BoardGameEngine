#include "frame.h"
#include "common.h"
#include "notify.h"
#include <stdlib.h>

#define MAX_FRAMES 1 << 16

static Rectangle *rects;
static uint16_t *counters;
static FrameAttr *attrs;
static int next_frame = 0;

static void on_user_frame_created(NotifyArgs args);

void init_frame_system()
{
  rects = calloc(MAX_FRAMES, sizeof(*rects));
  counters = calloc(MAX_FRAMES, sizeof(*rects));
  attrs = calloc(MAX_FRAMES, sizeof(*rects));
  register_listener(TEXTURE_FRAME_CREATED, &on_user_frame_created);
}

frame_h emerge_frame(Rectangle rect, FrameAttr attributes)
{
  uint32_t id = 0;
  set_msb(&id, counters[next_frame]);
  set_lsb(&id, next_frame);
  rects[next_frame] = rect;
  attrs[next_frame] = attributes;
  next_frame += 1;
}

void submerge_frame(frame_h frame_handler)
{
  rects[get_lsb(frame_handler.id)].x = 0;
  rects[get_lsb(frame_handler.id)].y = 0;
  rects[get_lsb(frame_handler.id)].width = 0;
  rects[get_lsb(frame_handler.id)].height = 0;
  next_frame = get_lsb(frame_handler.id);
  counters[get_msb(frame_handler.id)] += 1;
}

Rectangle *frame_pointer(frame_h frame_handler)
{
  if (counters[get_lsb(frame_handler.id)] != get_msb(frame_handler.id)) {
    return NULL;
  }
  return &rects[get_lsb(frame_handler.id)];
}

Rectangle *rectangle_list()
{
  return rects;
}

FrameAttr *attr_list()
{
  return attrs;
}

static void on_user_frame_created(NotifyArgs args)
{
  Vector2 world_coords = args.v2;
  Texture tex = args.tex;
  frame_h id = emerge_frame((Rectangle) {world_coords.x, world_coords.y, tex.width, tex.height},
			    (FrameAttr) {tex});
}