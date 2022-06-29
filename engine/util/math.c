#include "math.h"

float aabb_raycast(vec3 origin, vec3 dir, aabb_t box) {
  float t[10];
  t[1] = (box.min[0] - origin[0]) / dir[0];
  t[2] = (box.max[0] - origin[0]) / dir[0];
  t[3] = (box.min[1] - origin[1]) / dir[1];
  t[4] = (box.max[1] - origin[1]) / dir[1];
  t[5] = (box.min[2] - origin[2]) / dir[2];
  t[6] = (box.max[2] - origin[2]) / dir[2];
  t[7] = MAX(MAX(MIN(t[1], t[2]), MIN(t[3], t[4])), MIN(t[5], t[6]));
  t[8] = MIN(MIN(MAX(t[1], t[2]), MAX(t[3], t[4])), MAX(t[5], t[6]));
  t[9] = (t[8] < 0 || t[7] > t[8]) ? 0 : t[7];
  return t[9];
}
