#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <microtar.h>
#include <parson/parson.h>
#include <vec.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "log.h"

#define KEYBIND_FORWARD 0
#define KEYBIND_BACK 1
#define KEYBIND_LEFT 2
#define KEYBIND_RIGHT 3
#define KEYBIND_JUMP 4
#define KEYBIND_MENU 5
#define KEYBIND_CONSOLE 6
#define KEYBIND_DEBUG 7
#define KEYBINDS 8

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
  void* data;
  int len;
} asset_t;

typedef struct {
  float world_gravity;
  vec3 world_lightdir;
  float player_speed;
  float player_jumpheight;
  bool debug_drawcolliders;
} state_t;

extern conf_t conf;
extern state_t state;
extern const char* keybind_names_h[KEYBINDS];

void conf_init(const char* path);
void conf_write(const char* path);
void assets_init(const char* path);
asset_t asset_load(const char* path);
void log_game(const char* fmt, ...);
void lua_init();
void lua_exec(const char* buf);
