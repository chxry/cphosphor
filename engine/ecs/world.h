#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>
#include <parson/parson.h>
#include <vec.h>

#include "core/gfx.h"
#include "core/lighting.h"
#include "engine.h"
#include "log.h"

// move
typedef struct {
  vec3 min;
  vec3 max;
} aabb_t;

typedef struct {
  const char* name;
  vec3 pos;
  vec3 rot;
  vec3 scale;
} entity_t;
typedef vec_t(entity_t) vec_entity_t;

typedef struct {
  int entity;
} component_t;

typedef struct {
  component_t c;
  const char* mesh;
  const char* tex;
  // todo only in editor mode
  char mesh_buf[256];
  char tex_buf[256];
} model_t;
typedef vec_t(model_t) vec_model_t;

typedef struct {
  component_t c;
  aabb_t b;
} collider_t;
typedef vec_t(collider_t) vec_collider_t;

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

#define VEC3_FROM_JSON(v) \
  { json_array_get_number(v, 0), json_array_get_number(v, 1), json_array_get_number(v, 2) }
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

extern world_t world;

void world_load(const char* path);
void world_write(const char* path);
void world_render(mat4 view, mat4 projection);
void world_render_colliders(mat4 view, mat4 projection);
void world_render_collider(mat4 view, mat4 projection, int entity);
void world_render_shadows(mat4 view, mat4 projection);
bool world_test_collision(aabb_t box);
float aabb_raycast(vec3 origin, vec3 dir, aabb_t box);
float world_raycast(vec3 origin, vec3 dir);
