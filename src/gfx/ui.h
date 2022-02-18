#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <lua.h>

#include "game/player.h"
#include "game/game.h"
#include "gfx.h"

#define VEC2_ZERO \
  (ImVec2) { 0.0, 0.0 }

void ui_init();
void ui_processevent(SDL_Event* e);
void ui_render(SDL_Window* window, int w, int h);