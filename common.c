#include "common.h"
#include <raymath.h>

Vector2 get_mouse_delta() {
  Vector2 vec = Vector2Subtract(GetMousePosition(), previous_mouse_position);
  return vec;
}
