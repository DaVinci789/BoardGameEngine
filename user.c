#include "user.h"
#include "common.h"
#include "frame.h"
#include "notify.h"

#include <raymath.h>
#include <tinyfiledialogs.h>
#include <stddef.h>
#include <stdio.h>

static int over_card = 0;
static Vector2 selection_offset = {0};
static frame_h *selected_frames;
static int selected_frames_len = 0;
static void on_query_cards_finished(NotifyArgs args);

User user_init()
{
  User user = {0};
  user.cam.zoom = 1;
  user.selection_rec.x = -10000;
  user.selection_rec.y = -10000;
  register_listener(QUERY_CARD_FINISHED, &on_query_cards_finished);
  return user;
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
    notify(QUERY_CARD, (NotifyArgs) {.state = user->state,
				      .v2 = world_coords});
    if (over_card)
      user->state = GRABBING;
    else 
      user->state = SELECTING;

    notify(STATE_ENTERED, (NotifyArgs) {.state = user->state});
    return;
  }

  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    char const *patterns[2] = {"*.png", "*.jpg"};
    char *response = tinyfd_openFileDialog("Open File",
					   NULL,
					   2,
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
    over_card = 0;
    selection_offset = GetMousePosition();
    user->just_switched_state = 0;
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    user->just_switched_state = 1;
    user->state = HOVERING;
    notify(STATE_ENTERED, (NotifyArgs) {.state = user->state});
  }

  Vector2 position = GetMousePosition();
  for (int i = 0; i < selected_frames_len; i++) {
    Rectangle *r = get_rect(selected_frames[i]);
    r->x += position.x - selection_offset.x;
    r->y += position.y - selection_offset.y;
  }
  selection_offset = position;
}

static void on_query_cards_finished(NotifyArgs args)
{
  over_card = args.b;
  if (over_card) {
    selected_frames = args.frame_pointer;
    selected_frames_len = args.frame_array_len;
  }
}
