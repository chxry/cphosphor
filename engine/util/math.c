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

void quat_to_euler(vec4 q, vec3 e) {
  // x
  float sinr_cosp = 2 * (q[3] * q[0] + q[1] * q[2]);
  float cosr_cosp = 1 - 2 * (q[0] * q[0] + q[1] * q[1]);
  e[0] = atan2(sinr_cosp, cosr_cosp);

  // y
  float sinp = 2 * (q[3] * q[1] - q[2] * q[0]);
  if (fabs(sinp) >= 1) {
    e[1] = copysign(M_PI / 2, sinp);
  } else {
    e[1] = asin(sinp);
  }

  // z
  float siny_cosp = 2 * (q[3] * q[2] + q[0] * q[1]);
  float cosy_cosp = 1 - 2 * (q[1] * q[1] + q[2] * q[2]);
  e[2] = atan2(siny_cosp, cosy_cosp);
}
