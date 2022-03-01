#include "player.h"

vec3 player_pos = (vec3){0.0, 3.0, 3.0};
vec3 vel = (vec3){0.0, 0.0, 0.0};
vec3 cam_pos;
vec3 cam_dir;
float yaw = -90.0;
float pitch = 0.0;
bool ground = false;
const float HEIGHT = 1.6;
const float RADIUS = 0.4;

void player_processevent(SDL_Event* e) {
  if (!SDL_GetRelativeMouseMode()) {
    return;
  }
  switch (e->type) {
  case SDL_MOUSEMOTION:
    yaw += e->motion.xrel * conf.sens;
    pitch -= e->motion.yrel * conf.sens;
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (e->button.button == SDL_BUTTON_LEFT) {
      vec3 hit;
      float distance = world_raycast(cam_pos, cam_dir);
      if (distance == 0) {
        break;
      }
      glm_vec3_scale(cam_dir, distance, hit);
      glm_vec3_add(cam_pos, hit, hit);
      log_info("%f %f %f", hit[0], hit[1], hit[2]);
    }
    break;
  }
  if (pitch > 89.0) {
    pitch = 89.0;
  }
  if (pitch < -89.0) {
    pitch = -89.0;
  }
}

bool test_collision(vec3 new_pos) {
  vec3 player_box[2] = {{new_pos[0] - RADIUS, new_pos[1], new_pos[2] - RADIUS}, {new_pos[0] + RADIUS, new_pos[1] + HEIGHT, new_pos[2] + RADIUS}};
  bool collides = false;
  int i;
  collider_t collider;
  vec_foreach(&colliders, collider, i) {
    if ((collides = glm_aabb_aabb(player_box, (vec3*)&collider))) {
      break;
    }
  }
  if (collides) {
    glm_vec3_copy(player_pos, new_pos);
  } else {
    glm_vec3_copy(new_pos, player_pos);
  }
  return collides;
}

float aabb_raycast(vec3 origin, vec3 dir, collider_t box) {
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

float world_raycast(vec3 origin, vec3 dir) {
  float closest = 0;
  int i;
  collider_t collider;
  vec_foreach(&colliders, collider, i) {
    float distance = aabb_raycast(origin, dir, collider);
    if (distance == 0) {
      continue;
    }
    if (closest == 0) {
      closest = distance;
      continue;
    }
    closest = distance < closest ? distance : closest;
  }
  return closest;
}

void player_movement(mat4* view) {
  cam_dir[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  cam_dir[1] = sin(glm_rad(pitch));
  cam_dir[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_vec3_normalize(cam_dir);

  const unsigned char* keys = SDL_GetKeyboardState(NULL);
  if (SDL_GetRelativeMouseMode()) {
    if (keys[conf.binds[KEYBIND_FORWARD]]) {
      vel[0] += state.player_speed * cos(glm_rad(yaw));
      vel[2] += state.player_speed * sin(glm_rad(yaw));
    }
    if (keys[conf.binds[KEYBIND_BACK]]) {
      vel[0] -= state.player_speed * cos(glm_rad(yaw));
      vel[2] -= state.player_speed * sin(glm_rad(yaw));
    }
    if (keys[conf.binds[KEYBIND_LEFT]]) {
      vel[0] -= state.player_speed * cos(glm_rad(90 + yaw));
      vel[2] -= state.player_speed * sin(glm_rad(90 + yaw));
    }
    if (keys[conf.binds[KEYBIND_RIGHT]]) {
      vel[0] += state.player_speed * cos(glm_rad(90 + yaw));
      vel[2] += state.player_speed * sin(glm_rad(90 + yaw));
    }
    if (keys[conf.binds[KEYBIND_JUMP]] && ground) {
      vel[1] = state.player_jumpheight;
      ground = false;
    }
  }
  vel[1] -= state.world_gravity;
  vec3 new_pos;
  glm_vec3_copy(player_pos, new_pos);
  new_pos[0] += vel[0];
  test_collision(new_pos);

  new_pos[1] += vel[1];
  if (test_collision(new_pos)) {
    if (!ground && (ground = vel[1] < 0)) {
      audio_play("audio/fall.wav");
    }
    vel[1] = 0.0;
  }

  new_pos[2] += vel[2];
  test_collision(new_pos);
  vel[0] *= 0.9;
  vel[2] *= 0.9;

  glm_vec3_add(player_pos, (vec3){0.0, HEIGHT, 0.0}, cam_pos);
  glm_look(cam_pos, cam_dir, GLM_YUP, *view);
}