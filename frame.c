#include "frame.h"
#include "user.h"
#include "common.h"
#include "notify.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static Rectangle *rects;
static uint16_t *counters;
static FrameAttr *attrs;
static frame_h *frame_arena;
static int arena_used;
static int next_frame = 0;
static int user_hover = 0;

static void on_user_frame_created(NotifyArgs args);
static void on_user_state_enter(NotifyArgs args);
static void on_user_query_card(NotifyArgs args);

void init_frame_system()
{
  rects = calloc(MAX_FRAMES, sizeof(*rects));
  counters = calloc(MAX_FRAMES, sizeof(*counters));
  attrs = calloc(MAX_FRAMES, sizeof(*attrs));
  frame_arena = calloc(MAX_FRAMES, sizeof(*frame_arena));
  register_listener(TEXTURE_FRAME_CREATED, &on_user_frame_created);
  register_listener(STATE_ENTERED, &on_user_state_enter);
  register_listener(QUERY_CARD, &on_user_query_card);
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

void frame_update(Rectangle world_select)
{
  if (user_hover) {
    frame_h *frames = NULL;
    int frames_len = get_frames_colliding_world_rect(world_select, &frames);

    for (int i = 0; i < MAX_FRAMES; i++) {
      attrs[i].selected = 0;
    }

    for (int i = 0; i < frames_len; i++) {
      Rectangle r = *get_rect(frames[i]);
      get_attr(frames[i])->selected = CheckCollisionRecs(world_select, r);
    }
  }
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

static void on_user_state_enter(NotifyArgs args)
{
  user_hover = args.state == SELECTING;
}

static void on_user_query_card(NotifyArgs args)
{
  for (int i = 0; i < MAX_FRAMES; i++) {
    if (CheckCollisionRecs(rects[i],
			  (Rectangle) {
			    .x = args.v2.x,
			    .y = args.v2.y,
			    .width = 1,
			    .height = 1,
			  })) {
      notify(QUERY_CARD_FINISHED, (NotifyArgs) {.b = 1});
      return;
    }
  }
}
