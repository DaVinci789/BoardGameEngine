#include "frame.h"
#include "user.h"
#include "common.h"
#include "notify.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

static Rectangle *rects;
static uint16_t *counters;
static FrameAttr *attrs;
static frame_h *frame_arena;
static int arena_used;
static int next_frame = 0;

static void on_user_frame_created(NotifyArgs args);

void init_frame_system()
{
  rects = calloc(MAX_FRAMES, sizeof(*rects));
  counters = calloc(MAX_FRAMES, sizeof(*counters));
  attrs = calloc(MAX_FRAMES, sizeof(*attrs));
  frame_arena = calloc(MAX_FRAMES, sizeof(*frame_arena));
  register_listener(TEXTURE_FRAME_CREATED, &on_user_frame_created);
}

frame_h emerge_frame(Rectangle rect, FrameAttr attributes)
{
  uint32_t id = 0;
  set_msb(&id, counters[next_frame]);

  if (counters[next_frame] == USHRT_MAX); // TODO

  set_lsb(&id, next_frame);
  rects[next_frame] = rect;
  attrs[next_frame] = attributes;
  attrs[next_frame].id.id = id;
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

Rectangle *get_rect(frame_h h)
{
  if (counters[get_lsb(h.id)] != get_msb(h.id)) {
    return NULL;
  }
  return &rects[get_lsb(h.id)];
}

FrameAttr *get_attr(frame_h h)
{
  if (counters[get_lsb(h.id)] != get_msb(h.id)) {
    return NULL;
  }
  return &attrs[get_lsb(h.id)];
}

Rectangle *rectangle_list()
{
  return rects;
}

FrameAttr *attr_list()
{
  return attrs;
}

int get_frames_colliding_world_rect(Rectangle world_rect, frame_h **frames)
{
  int found = 0;
  frame_h *start = &frame_arena[arena_used];
  for (int i = 0; i < MAX_FRAMES; i++) {
    Rectangle r = rects[i];
    if (CheckCollisionRecs(world_rect, r)) {
      frame_arena[arena_used] = attrs[i].id;
      arena_used += 1;
      found += 1;
    }
  }
  if (found > 0) *frames = start;
  return found;
}

void reset_frame_arena()
{
  memset(frame_arena, 1, MAX_FRAMES);
  arena_used = 0;
}

static void on_user_frame_created(NotifyArgs args)
{
  Vector2 world_coords = args.v2;
  Texture tex = args.tex;
  frame_h id = emerge_frame((Rectangle) {world_coords.x, world_coords.y, tex.width, tex.height},
			    (FrameAttr) {.tex = tex, .c = WHITE});
}

void draw_frames()
{
  for (int i = 0; i < MAX_FRAMES; i++) {
    Rectangle rec = rects[i];
    Rectangle rec2 = rects[i];
    FrameAttr attr = attr_list()[i];
    rec.x = 0;
    rec.y = 0;
    DrawTextureRec(attr.tex, rec, (Vector2){rec2.x, rec2.y}, attr.selected ? BLACK : WHITE);
  }
}