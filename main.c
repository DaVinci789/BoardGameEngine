#include <raylib.h>
#include <raymath.h>
#include <tinyfiledialogs.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GRIDSIZE 16
#define MAX_FRAMES 1 << 16

Vector2 previous_mouse_position = {0};

typedef struct Frame {
  Rectangle rec;
  Texture tex;
  int hot;
  int active;
  int selected;
} Frame;

Frame *frames = NULL;
int used_frames = 0;

Texture
generate_grid()
{
  int gridsize = GRIDSIZE;
  uint32_t *data = malloc(sizeof(*data) * (gridsize * 2) * (gridsize * 2));
  char *curr = (char*) data;
  for (int y = 0; y < gridsize * 2; y++)
    {
      for (int x = 0; x < gridsize * 2; x++)
	{
	  Color current_color = WHITE;
	  if ((x % gridsize == 0 || x % gridsize == gridsize - 1) && (y % gridsize == 0 || y % gridsize == gridsize - 1))
	    current_color = BLUE;
	  curr[0] = (current_color.r);
	  curr[1] = (current_color.g);
	  curr[2] = (current_color.b);
	  curr[3] = (current_color.a);
	  curr += 4;
	}
    }
  Image image = {data, gridsize * 2, gridsize * 2, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
  return LoadTextureFromImage(image);
}

int main(void)
{
  frames = calloc(MAX_FRAMES, sizeof(Frame));
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "Board Games 2");
  Texture grid_texture = generate_grid();
  Vector2 hold_origin = {0};
  Vector2 hold_diff = {0};
  Vector2 selection_offset = {0};
  Rectangle selection_rec = {0};
  Camera2D cam = {0};

  Frame *over_card = NULL;
  int over_card_index = -1;

  int selection_rect_active = 0;
  int update_depth = 0;

  Frame **selected_cards = calloc(MAX_FRAMES, sizeof(Frame*));
  int selected_cards_len = 0;
  Frame **unselected_cards = calloc(MAX_FRAMES, sizeof(Frame*));
  int unselected_cards_len = 0;

  cam.zoom = 1;
  while (!WindowShouldClose()) {

  over_card = NULL;
  over_card_index = -1;
  memset(selected_cards, 0, MAX_FRAMES);
  selected_cards_len = 0;
  memset(unselected_cards, 0, MAX_FRAMES);
  unselected_cards_len = 0;
  for (int i = 0; i < MAX_FRAMES; i++) {
    Frame *f = &frames[i];
    if (CheckCollisionRecs(f->rec, (Rectangle) {
      .x = GetMouseX(),
      .y = GetMouseY(),
      .width = 1,
      .height = 1,
    })) {
      over_card = f;
      over_card_index = i;
    }

    if (selection_rect_active) {
      if (CheckCollisionRecs(f->rec, selection_rec)) {
	selected_cards[selected_cards_len] = f;
	selected_cards_len += 1;
      } else {
	unselected_cards[unselected_cards_len] = f;
	unselected_cards_len += 1;
      }
    } else {
      if (f->selected) {
	selected_cards[selected_cards_len] = f;
	selected_cards_len += 1;
      }
    }
  }

  if (over_card) over_card->hot = 1;
  for (int i = 0; i < selected_cards_len; i++) {
    selected_cards[i]->selected = 1;
  }
  for (int i = 0; i < unselected_cards_len; i++) {
    unselected_cards[i]->selected = 0;
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (over_card) {
      selection_offset = GetMousePosition();
      update_depth = 1;
    } else {
      for (int i = 0; i < selected_cards_len; i++) {
        selected_cards[i]->selected = 0;
      }
      hold_origin = GetMousePosition();
      selection_rect_active = 1;
    }
  } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    char const *patterns[1] = {"*.png"};
    char *response = tinyfd_openFileDialog("Open File",
					     NULL,
					     1,
					     patterns,
					     "image files",
					     0);
    if (response) {
      Vector2 world_coords = GetScreenToWorld2D(GetMousePosition(), cam);
      Texture tex = LoadTexture(response);
      Frame *f = &frames[used_frames];
      f->rec = (Rectangle) {.x = world_coords.x, .y = world_coords.y, .width = tex.width, .height = tex.height};
      f->tex = tex;
      used_frames += 1;
    }
  }

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    if (over_card && !selection_rect_active)  {
      over_card->hot = 0;
      if (hold_origin.x == 0) {
        over_card->rec.x += GetMouseX() - selection_offset.x;
        over_card->rec.y += GetMouseY() - selection_offset.y;
	if (!over_card->selected) {
	  for (int i = 0; i < selected_cards_len; i++) {
	    selected_cards[i]->selected = 0;
	  }
	};
        for (int i = 0; i < selected_cards_len; i++) {
          Frame *f = selected_cards[i];
          if (f == over_card) continue;
          f->rec.x += GetMouseX() - selection_offset.x;
          f->rec.y += GetMouseY() - selection_offset.y;
        }
        selection_offset = GetMousePosition();
      }
    } else {
      hold_diff = Vector2Add(hold_diff, Vector2Subtract(GetMousePosition(), previous_mouse_position));
      selection_rec.x = hold_origin.x;
      selection_rec.y = hold_origin.y;
      selection_rec.width = hold_diff.x;
      selection_rec.height = hold_diff.y;
      if (hold_diff.x < 0) {
        selection_rec.x = hold_origin.x + hold_diff.x;
        selection_rec.width *= -1;
      }
      if (hold_diff.y < 0) {
        selection_rec.y = hold_origin.y + hold_diff.y;
        selection_rec.height *= -1;
      }
    }
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    hold_diff.x = 0;
    hold_diff.y = 0;
    hold_origin.x = 0;
    hold_origin.y = 0;
    selection_rec.x = -10000;
    selection_rec.y = -10000;
    selection_rec.width = 0;
    selection_rec.height = 0;
    selection_rect_active = 0;
  }

  if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
    hold_diff = Vector2Add(hold_diff, Vector2Subtract(GetMousePosition(), previous_mouse_position));
    cam.offset = Vector2Add(cam.offset, hold_diff);
    hold_diff.x = 0;
    hold_diff.y = 0;
  }
  if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
    hold_diff.x = 0;
    hold_diff.y = 0;
  }
  previous_mouse_position = GetMousePosition();

  if (update_depth) {
    update_depth = 0;
    if (over_card_index != used_frames - 1) {
      int current_index = over_card_index;
      while (current_index != used_frames - 1) {
	Frame current_frame = frames[current_index];
	Frame next_frame = frames[current_index + 1];
	frames[current_index] = next_frame;
	frames[current_index + 1] = current_frame;
	current_index += 1;
      }
    }
  }

  BeginDrawing();
  BeginMode2D(cam);
  Rectangle src = (Rectangle){-100000, -100000, 200000, 200000};
  Rectangle dest = src;
  DrawTexturePro(grid_texture, src, dest, (Vector2){0}, 0, WHITE);
  for (int i = 0; i < MAX_FRAMES; i++) {
    Frame f = frames[i];
    Color c = WHITE;
    if (f.selected) c = YELLOW;
    if (f.hot) c = BLACK;
    DrawTextureRec(f.tex, (Rectangle) {0, 0, f.rec.width, f.rec.height}, (Vector2) {f.rec.x, f.rec.y}, c);
  }

  DrawRectangleLinesEx((Rectangle){
	  selection_rec.x,
	  selection_rec.y,
	  selection_rec.width,
	  selection_rec.height
	}, 1.0, BLUE);
  DrawRectangleRec((Rectangle){
	  selection_rec.x,
	  selection_rec.y,
	  selection_rec.width,
	  selection_rec.height
	}, (Color) {0, 0, 255, 127});

  EndMode2D();
  EndDrawing();
  if (over_card) over_card->hot = 0;
}
}
