#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>

#include "game.h"

extern vec3 player_pos;

void player_processevent(SDL_Event* e);
void player_movement(mat4* view);
