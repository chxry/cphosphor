#pragma once
#include <SDL2/SDL.h>
#include <cglm/cglm.h>

void player_processevent(SDL_Event* e);
void player_movement(mat4* view);
