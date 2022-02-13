#include "player.h"

vec3 pos = (vec3){0.0, 0.0, 3.0};
float yaw = -90.0;
float pitch = 0.0;
const float CAM_SPEED = 0.1;
const float CAM_SENS = 0.1;

void player_processevent(SDL_Event* e) {
  if (e->type == SDL_MOUSEMOTION) {
    yaw += e->motion.xrel * CAM_SENS;
    pitch -= e->motion.yrel * CAM_SENS;
  }
}

void player_movement(mat4* view) {
  vec3 front;
  front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  front[1] = sin(glm_rad(pitch));
  front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_vec3_normalize(front);

  const unsigned char* keys = SDL_GetKeyboardState(NULL);
  vec3 vel;
  if (keys[SDL_SCANCODE_W]) {
    glm_vec3_scale(front, CAM_SPEED, vel);
    glm_vec3_add(pos, vel, pos);
  }
  if (keys[SDL_SCANCODE_S]) {
    glm_vec3_scale(front, CAM_SPEED, vel);
    glm_vec3_sub(pos, vel, pos);
  }
  if (keys[SDL_SCANCODE_A]) {
    glm_vec3_crossn(front, GLM_YUP, vel);
    glm_vec3_scale(vel, CAM_SPEED, vel);
    glm_vec3_sub(pos, vel, pos);
  }
  if (keys[SDL_SCANCODE_D]) {
    glm_vec3_crossn(front, GLM_YUP, vel);
    glm_vec3_scale(vel, CAM_SPEED, vel);
    glm_vec3_add(pos, vel, pos);
  }

  glm_look(pos, front, GLM_YUP, *view);
}