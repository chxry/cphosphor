#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <vec.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "ecs/ecs.h"
#include "util/math.h"
#include "util/json.h"

typedef struct {
  int entity;
  aabb_t b;
} collider_t;
typedef vec_t(collider_t) vec_collider_t;

extern component_t collider;