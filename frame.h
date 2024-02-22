#ifndef FRAME_H
#define FRAME_H

#include <raylib.h>
#include <stdint.h>

#define MAX_FRAMES 1 << 16

typedef struct frame_h {uint32_t id;} frame_h;

typedef struct FrameAttr {
  frame_h id;
  int selected;
  Color c;
  Texture2D tex;
} FrameAttr;

void init_frame_system();
frame_h emerge_frame(Rectangle rect, FrameAttr attrs);
void submerge_frame(frame_h frame_handler);
void frame_update(Rectangle rect);
Rectangle *rectangle_list();
FrameAttr *attr_list();
Rectangle *get_rect(frame_h h);
FrameAttr *get_attr(frame_h h);
int get_frames_colliding_world_rect(Rectangle world_rect, frame_h **frames);
void reset_frame_arena();

#endif
