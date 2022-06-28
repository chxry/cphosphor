#pragma once
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <parson/parson.h>
#include <cglm/cglm.h>

#include "engine/engine.h"
#include "engine/log.h"

#define KEYBIND_FORWARD 0
#define KEYBIND_BACK 1
#define KEYBIND_LEFT 2
#define KEYBIND_RIGHT 3
#define KEYBIND_JUMP 4
#define KEYBIND_CROUCH 5
#define KEYBIND_SPRINT 6
#define KEYBIND_MENU 7
#define KEYBIND_CONSOLE 8
#define KEYBIND_DEBUG 9
#define KEYBINDS 10

typedef struct {
  int width;
  int height;
  bool fullscreen;
  int fps;
  float fov;
  float sens;
  SDL_Scancode binds[KEYBINDS];
  float volume;
  int crosshair_size;
  int crosshair_thickness;
  int crosshair_color;
} conf_t;

typedef struct {
  float world_gravity;
  float player_speed;
  float player_jumpheight;
  bool debug_drawcolliders;
} state_t;

extern conf_t conf;
extern state_t state;
extern const char* keybind_names_h[KEYBINDS];

void state_bind();
void conf_load(const char* path);
void conf_write(const char* path);
