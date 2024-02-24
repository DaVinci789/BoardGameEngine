#include "user.h"
#include "common.h"
#include "frame.h"
#include "notify.h"
#include "bucket.h"

#include <raymath.h>
#include <tinyfiledialogs.h>
#include <stddef.h>
#include <stdio.h>

static Vector2 selection_offset = {0};
static int selected_frames_len = 0;

static int get_hovered_cards(Rectangle r, FrameAttr a, void *context);
static int get_selected_cards(Rectangle r, FrameAttr a, void *context);
static int get_over_card(Rectangle r, FrameAttr a, void *context);

static Bucket *hovered_cards = NULL;
static Bucket *selected_cards = NULL;
static Bucket *over_card = NULL;

User user_init()
{
  User user = {0};
  user.cam.zoom = 1;
  user.selection_rec.x = -10000;
  user.selection_rec.y = -10000;
  return user;
}

void user_register_buckets(User *user)
{
  hovered_cards = register_bucket(&get_hovered_cards, user);
  selected_cards = register_bucket(&get_selected_cards, NULL);
  over_card = register_bucket(&get_over_card, NULL);
}

void hovering_update(User *user)
{
  if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
    Vector2 mouse_move = Vector2Multiply(get_mouse_delta(), (Vector2) {.x = (1.0 / user->cam.zoom), .y = (1.0 / user->cam.zoom)});
    user->cam.target = Vector2Subtract(user->cam.target, mouse_move);
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    user->just_switched_state = 1;
    Vector2 world_coords = GetScreenToWorld2D(GetMousePosition(), user->cam);

    if (over_card->len > 0)
      user->state = GRABBING;
    else 
      user->state = SELECTING;

    notify(STATE_ENTERED, (NotifyArgs) {.state = user->state});
    return;
  }

  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    char const *patterns[1] = {"*.png"};
    char *response = tinyfd_openFileDialog("Open File",
					   NULL,
					   1,
					   patterns,
					   "image files",
					   0);
    if (response) {
      Vector2 world_coords = GetScreenToWorld2D(GetMousePosition(), user->cam);
      Texture tex = LoadTexture(response);
      notify(TEXTURE_FRAME_CREATED, (NotifyArgs) {.v2 = world_coords, .tex = tex});
      return;
    }
  }
}

void selecting_update(User *user)
{
  if (user->just_switched_state) {
    user->hold_origin = GetMousePosition();
    user->just_switched_state = 0; // @FIXME: Ideally... the calling function wouldn't be responsible for switching back the state... it would be somewhere in main...
  }

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    user->hold_diff = Vector2Add(user->hold_diff, get_mouse_delta());
    user->selection_rec.x = user->hold_origin.x;
    user->selection_rec.y = user->hold_origin.y;
    user->selection_rec.width = user->hold_diff.x;
    user->selection_rec.height = user->hold_diff.y;
    if (user->hold_diff.x < 0) {
      user->selection_rec.x = user->hold_origin.x + user->hold_diff.x;
      user->selection_rec.width *= -1;
    }
    if (user->hold_diff.y < 0) {
      user->selection_rec.y = user->hold_origin.y + user->hold_diff.y;
      user->selection_rec.height *= -1;
    }
  } // selection_rec used in frame_update(Rect);

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    user->just_switched_state = 1;
    user->state = HOVERING;
    user->hold_diff.x = 0;
    user->hold_diff.y = 0;
    user->selection_rec.x = -10000;
    user->selection_rec.y = -10000;
    user->selection_rec.width = 0;
    user->selection_rec.height = 0;
    notify(STATE_ENTERED, (NotifyArgs) {.state = user->state});
  }
}

void grabbing_update(User *user)
{
  if (user->just_switched_state) {
    user->just_switched_state = 0;
    selection_offset = GetMousePosition();
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    user->just_switched_state = 1;
    user->state = HOVERING;
    notify(STATE_ENTERED, (NotifyArgs) {.state = user->state});
  }

  Vector2 position = GetMousePosition();
  frame_h *start = get_bucket(*selected_cards);
  for (int i = 0; i < selected_cards->len; i++) {
    Rectangle *r = get_rect(start[i]);
    FrameAttr *a = get_attr(start[i]);
    printf("id: %d\n", a->id);
    r->x += position.x - selection_offset.x;
    r->y += position.y - selection_offset.y;
  }
  selection_offset = position;
}

static int get_over_card(Rectangle r, FrameAttr a, void *context)
{
  return CheckCollisionRecs(r, (Rectangle) {
    .x = GetMouseX(),
    .y = GetMouseY(),
    .width = 1,
    .height = 1,
  });
}

static int get_hovered_cards(Rectangle r, FrameAttr a, void *context)
{
  User *user = (User *) context;
  return CheckCollisionRecs(r, user->selection_rec);
}

static int get_selected_cards(Rectangle r, FrameAttr a, void *context)
{
  return a.selected;
}