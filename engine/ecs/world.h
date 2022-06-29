#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>
#include <parson/parson.h>
#include <vec.h>

#include "components/model.h"
#include "components/collider.h"
#include "core/gfx.h"
#include "core/lighting.h"
#include "util/math.h"
#include "util/json.h"
#include "engine.h"
#include "log.h"

typedef struct {
  const char* name;
  int id;
  vec3 pos;
  vec3 rot;
  vec3 scale;
} entity_t;
typedef vec_t(entity_t) vec_entity_t;

typedef enum {
  skybox,
  atmosphere
} sky_mode_t;

typedef struct {
  vec_entity_t entities;
  vec_model_t models;
  vec_collider_t colliders;
  float light_ambient;
  vec3 light_dir;
  vec3 light_color;
  sky_mode_t sky_mode;
} world_t;

extern world_t world;

void world_load(const char* path);
void world_write(const char* path);
entity_t* get_entity(int id);
void world_render(mat4 view, mat4 projection);
void world_render_colliders(mat4 view, mat4 projection);
void world_render_collider(mat4 view, mat4 projection, int entity);
void world_render_shadows(mat4 view, mat4 projection);
bool world_test_collision(aabb_t box);
float world_raycast(vec3 origin, vec3 dir);
