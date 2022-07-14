#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <iconfonts/IconsFontAwesome4.h>

#include "core/physics.h"
#include "ecs/world.h"
#include "util/ui.h"

typedef struct {
  int entity;
  collider_t collider;
  float mass;
  float friction;
  float bounciness;
  void* body;
} rigidbody_t;

extern component_t rigidbody;