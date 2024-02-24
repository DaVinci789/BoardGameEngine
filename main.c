#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "user.h"
#include "frame.h"
#include "notify.h"
#include "bucket.h"

#define GRIDSIZE 16

Vector2 previous_mouse_position = {0};

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
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "Board Games");

  Texture grid_texture = generate_grid();

  init_notification_system();
  init_frame_system();
  init_buckets();

  User user = user_init();
  user_register_buckets(&user);

  while (!WindowShouldClose())
    {
      update_buckets();
      switch (user.state) {
      case HOVERING:
	hovering_update(&user);
	break;
      case SELECTING:
	selecting_update(&user);
	break;
      case GRABBING:
	grabbing_update(&user);
	break;
      case MENU:
	break;
      default:
	break;
      }
      previous_mouse_position = GetMousePosition();

      BeginDrawing();
      ClearBackground(RAYWHITE);

      BeginMode2D(user.cam);

      Rectangle src = (Rectangle){-100000, -100000, 200000, 200000};
      Rectangle dest = src;
      DrawTexturePro(grid_texture, src, dest, (Vector2){0}, 0, WHITE);

      draw_frames();

      EndMode2D();

      if (user.state == SELECTING) {
	DrawRectangleLinesEx((Rectangle){
	    user.selection_rec.x,
	    user.selection_rec.y,
	    user.selection_rec.width,
	    user.selection_rec.height
	  }, 1.0, BLUE);
	DrawRectangleRec((Rectangle){
	    user.selection_rec.x,
	    user.selection_rec.y,
	    user.selection_rec.width,
	    user.selection_rec.height
	  }, (Color) {0, 0, 255, 127});
      }

      switch (user.state) {
      case HOVERING:
	DrawText("HOVERING", 0, 0, 12, BLACK);
	break;
      case GRABBING:
	DrawText("GRABBING", 0, 0, 12, BLACK);
	break;
      case SELECTING:
	DrawText("SELECTING", 0, 0, 12, BLACK);
	break;
      default:
	break;
      }
      EndDrawing();
      reset_buckets();
    }
  CloseWindow();
}
