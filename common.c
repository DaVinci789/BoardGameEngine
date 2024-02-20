#include "common.h"
#include <raymath.h>

Vector2 get_mouse_delta() {
  Vector2 vec = Vector2Subtract(GetMousePosition(), previous_mouse_position);
  return vec;
}

uint16_t get_msb(uint32_t value)
{
  return (uint16_t) ((value & 0xFFFF0000) >> 4);
}

uint16_t get_lsb(uint32_t value)
{
  return (uint16_t) (value & 0x0000FFFF);
}

void set_msb(uint32_t *value, uint16_t set)
{
  uint32_t temp = ((uint32_t) set) << 16;
  uint32_t temp2 = get_lsb(*value);
  *value = temp | temp2;
}

void set_lsb(uint32_t *value, uint16_t set)
{
  uint32_t temp = (uint32_t) set;
  uint32_t temp2 = ((uint32_t) get_msb(*value)) << 16;
  *value = temp2 | temp;
}
