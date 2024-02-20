#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "user.h"
#include "frame.h"

#define GRIDSIZE 16

Vector2 previous_mouse_position = {0};
Texture tex;

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
  User user = {0};
  user.cam.zoom = 1;
  user.selection_rec.x = -10000;
  user.selection_rec.y = -10000;

  init_frame_system();
  while (!WindowShouldClose())
    {
      switch (user.state) {
      case HOVERING:
	hovering_update(&user);
	break;
      case SELECTING:
	selecting_update(&user);
	break;
      case GRABBING:
	break;
      case MENU:
	break;
      default:
	break;
      }
      previous_mouse_position = GetMousePosition();

      /* if (IsKeyPressed(KEY_N)) { */
      /* 	frame_h the_frame = emerge_frame((Rectangle){ */
      /* 	    .x = GetMouseX(), */
      /* 	    .y = GetMouseY(), */
      /* 	    .width = 30, */
      /* 	    .height = 30, */
      /* 	}); */
      /* } */

      BeginDrawing();
      ClearBackground(RAYWHITE);

      BeginMode2D(user.cam);

      Rectangle src = (Rectangle){-100000, -100000, 200000, 200000};
      Rectangle dest = src;
      DrawTexturePro(grid_texture, src, dest, (Vector2){0}, 0, WHITE);
      
      if (tex.id != 0) {
	DrawTexture(tex, 0, 0, WHITE);
      }

      Vector2 world_coords = GetScreenToWorld2D((Vector2) {user.selection_rec.x, user.selection_rec.y}, user.cam);
      Vector2 world_size = Vector2Multiply((Vector2) {user.selection_rec.width, user.selection_rec.height},
					   (Vector2) {(1.0/user.cam.zoom), (1.0/user.cam.zoom)});
      Rectangle selected_world_rect = {world_coords.x, world_coords.y, world_size.x, world_size.y};
      for (int i = 0; i < 1 << 16; i++) {
	Color c = CheckCollisionRecs(selected_world_rect, rectangle_list()[i]) ? BLACK : WHITE;
	Rectangle rec = rectangle_list()[i];
	Rectangle rec2 = rectangle_list()[i];
	rec.x = 0;
	rec.y = 0;
	DrawTextureRec(attr_list()[i].tex, rec, (Vector2) {rec2.x, rec2.y}, c);
      }

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

      EndDrawing();
    }
  CloseWindow();
}
