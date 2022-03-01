#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>
#include <parson/parson.h>
#include <vec.h>

#include "core.h"
#include "gfx.h"
#include "lighting.h"

typedef struct {
  const char* mesh;
  const char* tex;
  vec3 pos;
  vec3 rot;
  vec3 scale;
} gameobj_t;

typedef struct {
  vec3 min;
  vec3 max;
} collider_t;

#define VEC3_FROM_JSON(v) \
  { json_array_get_number(v, 0), json_array_get_number(v, 1), json_array_get_number(v, 2) }

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void world_init();
void world_render(mat4 view, mat4 projection);
void world_render_colliders(mat4 view, mat4 projection);
void world_render_shadows(mat4 view, mat4 projection);
bool world_test_collision(collider_t box);
float aabb_raycast(vec3 origin, vec3 dir, collider_t box);
float world_raycast(vec3 origin, vec3 dir);
