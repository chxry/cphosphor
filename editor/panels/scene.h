#pragma once
#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cglm/cglm.h>

#include "engine/gfx.h"
#include "engine/lighting.h"
#include "engine/world.h"

void scene_init();
void scene_processevent(SDL_Event* e);
void scene_update();
void scene_render();
