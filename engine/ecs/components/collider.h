#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <vec.h>

#include "util/math.h"
#include "util/json.h"

typedef struct {
  int entity;
  aabb_t b;
} collider_t;
typedef vec_t(collider_t) vec_collider_t;

collider_t collider_load(JSON_Object* obj);
JSON_Value* collider_save(collider_t collider);
void collider_inspector(collider_t* collider, int i);
collider_t collider_create(int entity);