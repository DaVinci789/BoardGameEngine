#ifndef USER_H
#define USER_H

#include <raylib.h>

typedef enum UserState {
  HOVERING,
  SELECTING,
  GRABBING,
  MENU,
} UserState;

typedef struct User {
  UserState state;
  int just_switched_state;
  Camera2D cam;

  Vector2 hold_origin;
  Vector2 hold_diff;
  Rectangle selection_rec;
} User;

User user_init();
void hovering_update(User *user);
void selecting_update(User *user);

#endif
