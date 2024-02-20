#ifndef FRAME_H
#define FRAME_H

#include <raylib.h>
#include <stdint.h>

typedef struct frame_h {uint32_t id;} frame_h;

typedef struct FrameAttr {
  Texture2D tex;
} FrameAttr;

void init_frame_system();
frame_h emerge_frame(Rectangle rect, FrameAttr attrs);
void submerge_frame(frame_h frame_handler);
Rectangle *frame_pointer(frame_h frame_handler);
Rectangle *rectangle_list();
FrameAttr *attr_list();

#endif
