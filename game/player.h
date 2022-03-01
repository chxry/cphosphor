#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <vec.h>

#include "engine/audio.h"
#include "game/world.h"
#include "game/game.h"

extern vec3 player_pos;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void player_processevent(SDL_Event* e);
float aabb_raycast(vec3 origin, vec3 dir, collider_t box);
float world_raycast(vec3 origin, vec3 dir);
void player_movement(mat4* view);
