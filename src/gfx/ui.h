#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

#include "game/game.h"
#include "gfx.h"

#define LILAC (ImVec4){0.3102, 0.2956, 0.3942, 1.0};
#define LILAC2 (ImVec4){0.2953, 0.2716, 0.4332, 1.0};
#define LILAC3 (ImVec4){0.2941, 0.2724, 0.4334, 1.0};
#define VEC2_ZERO \
  (ImVec2) { 0.0, 0.0 }

void ui_init();
void ui_processevent(SDL_Event* e);
void ui_render(SDL_Window* window);