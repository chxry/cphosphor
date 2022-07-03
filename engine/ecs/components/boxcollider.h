#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "ecs/world.h"
#include "util/math.h"
#include "util/json.h"

typedef struct {
  int entity;
  aabb_t b;
} boxcollider_t;

extern component_t boxcollider;