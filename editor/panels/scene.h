#pragma once
#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>
#include <cglm/cglm.h>

#include "engine/core/gfx.h"
#include "engine/core/lighting.h"
#include "engine/ecs/world.h"
#include "editor.h"

#define SCENE_TITLE ICON_FA_PICTURE_O " Scene"

void scene_init();
void scene_processevent(SDL_Event* e);
void scene_update();
void scene_render();
