#pragma once
#include <cglm/cglm.h>
#include <math.h>

typedef struct {
  vec3 min;
  vec3 max;
} aabb_t;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

float aabb_raycast(vec3 origin, vec3 dir, aabb_t box);
void quat_to_euler(vec4 q, vec3 e);
