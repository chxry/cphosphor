#pragma once
#include <cglm/cglm.h>
#include <parson/parson.h>
#include <map.h>
#include <vec.h>

#include "gfx/gfx.h"

typedef struct {
  const char* mesh;
  const char* tex;
  vec3 pos;
  vec3 rot;
  vec3 scale;
} gameobj_t;

#define VEC3_FROM_JSON(v) \
  { json_array_get_number(v, 0), json_array_get_number(v, 1), json_array_get_number(v, 2) }

void world_init();
void world_render(mat4 view, mat4 projection);
