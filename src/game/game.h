#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <microtar.h>
#include <parson/parson.h>
#include <log.h>

#define KEYBIND_FORWARD 0
#define KEYBIND_BACK 1
#define KEYBIND_LEFT 2
#define KEYBIND_RIGHT 3
#define KEYBIND_JUMP 4
#define KEYBIND_DEBUG 5
#define KEYBIND_MENU 6
#define KEYBINDS 7

typedef struct {
  int width;
  int height;
  bool fullscreen;
  int msaa;
  float fov;
  float sens;
  SDL_Scancode binds[KEYBINDS];
} conf_t;

typedef struct {
  void* data;
  int len;
} asset_t;

extern conf_t conf;
extern const char* keybind_names_h[KEYBINDS];

void conf_init(const char* path);
void conf_write(const char* path);
void assets_init(const char* path);
asset_t asset_load(const char* path);
