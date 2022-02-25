#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>
#include <parson/parson.h>
#include <map.h>
#include <vec.h>

#include "game.h"

typedef struct {
  const char* mesh;
  const char* tex;
  vec3 pos;
  vec3 rot;
  vec3 scale;
} gameobj_t;
typedef vec_t(gameobj_t) vec_gameobj_t;

typedef struct {
  vec3 min;
  vec3 max;
} collider_t;
typedef vec_t(collider_t) vec_collider_t;

#include "gfx/gfx.h"

#define VEC3_FROM_JSON(v) \
  { json_array_get_number(v, 0), json_array_get_number(v, 1), json_array_get_number(v, 2) }

extern vec_collider_t colliders;

void world_init();
void world_render(mat4 view, mat4 projection);
void world_render_shadows(mat4 view, mat4 projection);
