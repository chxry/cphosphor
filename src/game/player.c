#include "player.h"

vec3 player_pos = (vec3){0.0, 3.0, 3.0};
float yaw = -90.0;
float pitch = 0.0;
float yvel = 0.0;
bool ground = true;
const float SPEED = 0.1;
const float GRAVITY = 0.005;
const float HEIGHT = 1.6;

void player_processevent(SDL_Event* e) {
  if (e->type == SDL_MOUSEMOTION && SDL_GetRelativeMouseMode()) {
    yaw += e->motion.xrel * conf.sens;
    pitch -= e->motion.yrel * conf.sens;
  }
  if (pitch > 89.0) {
    pitch = 89.0;
  }
  if (pitch < -89.0) {
    pitch = -89.0;
  }
}

void player_movement(mat4* view) {
  vec3 front;
  front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  front[1] = sin(glm_rad(pitch));
  front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_vec3_normalize(front);

  const unsigned char* keys = SDL_GetKeyboardState(NULL);
  vec3 vel = GLM_VEC3_ZERO;
  if (SDL_GetRelativeMouseMode()) {
    if (keys[conf.binds[KEYBIND_FORWARD]] && !keys[conf.binds[KEYBIND_BACKWARD]]) {
      glm_vec3_scale(front, SPEED, vel);
    }
    if (keys[conf.binds[KEYBIND_BACKWARD]] && !keys[conf.binds[KEYBIND_FORWARD]]) {
      glm_vec3_scale(front, -SPEED, vel);
    }
    if (keys[conf.binds[KEYBIND_LEFT]] && !keys[conf.binds[KEYBIND_RIGHT]]) {
      glm_vec3_crossn(front, GLM_YUP, vel);
      glm_vec3_scale(vel, -SPEED, vel);
    }
    if (keys[conf.binds[KEYBIND_RIGHT]] && !keys[conf.binds[KEYBIND_LEFT]]) {
      glm_vec3_crossn(front, GLM_YUP, vel);
      glm_vec3_scale(vel, SPEED, vel);
    }
    if (keys[conf.binds[KEYBIND_JUMP]] && ground) {
      yvel = 0.2;
      ground = false;
    }
  }
  yvel -= GRAVITY;
  vel[1] = yvel;
  glm_vec3_add(player_pos, vel, player_pos);
  if (player_pos[1] < 0.0) {
    player_pos[1] = 0.0;
    ground = true;
  }

  vec3 cam_pos;
  glm_vec3_add(player_pos, (vec3){0.0, HEIGHT, 0.0}, cam_pos);
  glm_look(cam_pos, front, GLM_YUP, *view);
}