#ifndef COMMON_H
#define COMMON_H

#include <raylib.h>
#include <stdint.h>

extern Vector2 previous_mouse_position;

Vector2 get_mouse_delta();
uint16_t get_msb(uint32_t value);
uint16_t get_lsb(uint32_t value);
void set_msb(uint32_t *value, uint16_t set);
void set_lsb(uint32_t *value, uint16_t set);

#endif
