#pragma once
#include <capi/capi.h>
#include <cglm/cglm.h>
#include <vec.h>

typedef enum {
  collider_cube,
  collider_sphere,
  collider_cylinder
} collider_t;

#include "ecs/world.h"
#include "util/math.h"

void physics_init();
void physics_update();