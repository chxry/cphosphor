#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>
#include <parson/parson.h>
#include <vec.h>
#include <map.h>

#include "core/gfx.h"
#include "core/renderer.h"
#include "util/math.h"
#include "util/json.h"
#include "log.h"

typedef struct {
  vec_void_t components;
  void* (*load)(JSON_Object* obj);
  JSON_Value* (*save)(void* c);
  void (*inspector)(void* c, int i);
  void* (*create)(int entity);
  char* inspector_label;
} component_t;

#include "components/model.h"
#include "components/boxcollider.h"
#include "components/light.h"
#include "components/rigidbody.h"
#include "components/audiosrc.h"
#include "components/luascript.h"

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
  map_t(component_t) components;
  float light_ambient;
  float light_diffuse;
  vec3 light_dir;
  vec3 light_color;
  int shadow_softness;
  sky_mode_t sky_mode;
} world_t;

typedef struct {
  entity_t* entity;
  float distance;
  vec3 hit;
} collision_t;

extern world_t world;

void world_load(const char* path);
void world_write(const char* path);
void component_register(char* name, component_t component);
void component_iter(void (*f)(component_t* c, char* name));
entity_t* get_entity(int id);
component_t* get_component(char* name);
void entity_delete(int id);
bool world_test_collision(aabb_t box);
collision_t world_raycast(vec3 origin, vec3 dir);
