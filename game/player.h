#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>

#include "engine/core/audio.h"
#include "engine/ecs/world.h"
#include "game/game.h"

extern vec3 player_pos;
extern vec3 cam_dir;

void player_processevent(SDL_Event* e);
void player_movement(mat4* view);
