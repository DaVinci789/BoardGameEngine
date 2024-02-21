#include "user.h"
#include "common.h"
#include "frame.h"
#include "notify.h"

#include <raymath.h>
#include <tinyfiledialogs.h>
#include <stddef.h>

void hovering_update(User *user)
{
  if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
    Vector2 mouse_move = Vector2Multiply(get_mouse_delta(), (Vector2) {.x = (1.0 / user->cam.zoom), .y = (1.0 / user->cam.zoom)});
    user->cam.target = Vector2Subtract(user->cam.target, mouse_move);
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    user->just_switched_state = 1;
    user->state = SELECTING;
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
    }
  }
}

void selecting_update(User *user)
{
  if (user->just_switched_state) {
    user->hold_origin = GetMousePosition();
    user->just_switched_state = 0;
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
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    user->just_switched_state = 1;
    user->state = HOVERING;
    user->hold_diff.x = 0;
    user->hold_diff.y = 0;
    user->selection_rec.x = -10000;
    user->selection_rec.y = -10000;
    user->selection_rec.width = 0;
    user->selection_rec.height = 0;
  }
}
